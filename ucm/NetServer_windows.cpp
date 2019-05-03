#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <libutl/BufferedStream.h>
#include <libutl/HostOS.h>
#include <libutl/LogMgr.h>
#include <libutl/NetServer_windows.h>
#include <libutl/TCPsocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
//#define DEBUG_MODULE
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::NetServer);
UTL_CLASS_IMPL(utl::NetServerClient);

////////////////////////////////////////////////////////////////////////////////////////////////////

template class utl::RingBuffer<utl::NetServerClient*>;
template class std::function<void(utl::NetServerClient*)>;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NetServer //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void*
NetServer::run(void* arg)
{
    while (!exiting())
    {
        try
        {
            listen();
        }
        catch (Exception&)
        {
            break;
        }
    }

    // get rid of any connected clients
    clientDisconnectAll();

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
NetServer::addServer(TCPserverSocket* serverSocket)
{
    if (!_serverSockets.add(serverSocket))
        return false;
    int fd = serverSocket->fd();
    ASSERTD(fd >= 0);
    FD_SET(fd, &_serverFDs);
    _maxFD = max(_maxFD, fd);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::listen()
{
    super::testCancel();

    // one millisecond seems to be enough to avoid hammering CPU
    // note: 1000 usec   = 1 ms
    //       100000 usec = 100 ms (1/10 sec)
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 200000;

    // select()
    ASSERTD(_maxFD >= 0);
    fd_set rset = _serverFDs;
    UTL_EINTR_LOOP(select(_maxFD + 1, &rset, nullptr, nullptr, &tv));

    // nothing happened?
    if (err == 0)
        return;

    // error?
    if (err < 0)
    {
        h_errToEx();
    }

    // handle socket events
    uint_t i, lim = rset.fd_count;
    for (i = 0; i != lim; ++i)
    {
        int fd = (int)rset.fd_array[i];

        // accept the incoming socket connection
        auto ss = serverFind(fd);
        ASSERTD(ss != nullptr);
        auto clientSocket = new TCPsocket;
        InetHostAddress clientAddr;
        if (!ss->accept(clientSocket, clientAddr))
        {
            delete clientSocket;
            continue;
        }

        // too many clients?
        if (_clientsCount.fetch_add(1, std::memory_order_relaxed) >= _clientsMax)
        {
            // write "server busy" message, and ditch the connection
            _clientsCount.fetch_sub(1, std::memory_order_relaxed);
            try
            {
                clientWriteServerBusyMsg(*clientSocket);
            }
            catch (Exception&)
            {
            }
            delete clientSocket;
            continue;
        }

        // make or re-use a thread for the client
        NetServerClient* thread;
        if (_paused.deQ(thread))
        {
            thread->resume(clientSocket, clientAddr);
            onClientConnect(thread);
            thread->wakeup();
        }
        else
        {
            thread = clientMake(clientSocket, clientAddr);
            thread->setNumClients(_clientsPerThread);
            clientsAdd(thread);
            onClientConnect(thread);
            thread->start(nullptr, true);
        }
        if (_joinable.count() > 0)
            cleanupJoinable();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::clientDisconnectAll()
{
    // tell the active client threads to exit
    for (size_t i = 0; i != _clientsSize; ++i)
    {
        auto client = _clients[i].load(std::memory_order_relaxed);
        if (client == nullptr)
            continue;
        client->exit();
    }

    // now wait until clients are all gone
    while ((_clientsCount > 0) || (_paused.count() > 0) || (_joinable.count() > 0))
    {
        hostOS->usleep(100000);
        cleanupPaused();
        cleanupJoinable();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::clientDisconnect(NetServerClient* client)
{
    client->exit();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NetServerClient*
NetServer::clientMake(FDstream* socket, const InetHostAddress& addr)
{
    return new NetServerClient(this, socket, addr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
NetServer::exiting() const
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
void
NetServer::init(size_t maxClients, size_t maxPaused, size_t clientsPerThread)
{
    _serverSockets.setMultiSet(false);
    _serverSockets.setSorted(false);
    FD_ZERO(&_serverFDs);
    _maxFD = -1;
    _clientsPerThread = clientsPerThread;

    // _clients
    _clientsSize = 1;
    while (_clientsSize < maxClients)
        _clientsSize <<= 1;
    _clientsSize <<= 1;
    _clientsMask = _clientsSize - 1;
    _clientsCount = 0;
    _clientsMax = maxClients;
    _clientsIndex = 0;

    // put an empty cache line on each end of the _clients[] allocation
    size_t clientsSizeBytes =
        (_clientsSize * sizeof(client_ptr_t)) + (2 * UTL_ARCH_CACHE_LINE_SIZE);
    _clients = (client_ptr_t*)malloc(clientsSizeBytes);
    _clients += (UTL_ARCH_CACHE_LINE_SIZE / sizeof(client_ptr_t));
    auto lim = _clients + _clientsSize;
    for (auto it = _clients; it != lim; ++it)
    {
        new (it) client_ptr_t(nullptr);
    }

    // _paused & _joinable
    _paused.set(maxClients);
    _joinable.set(maxClients);
}
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::deInit()
{
    clientDisconnectAll();
    _clients -= (UTL_ARCH_CACHE_LINE_SIZE / sizeof(client_ptr_t));
    free(_clients);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
NetServer::clientDeactivate(NetServerClient* client)
{
    // one less active client
    size_t& numClients = client->_numClients;

    // paused client is now becoming joinable?
    if (numClients == size_t_max)
    {
        numClients = 0;
        _joinable.enQ(client);
        return true;
    }

    // the client was active (not paused)
    onClientDisconnect(client);
    client->pause();
    _clientsCount.fetch_sub(1, std::memory_order_relaxed);

    // pause the client if possible
    if ((--numClients > 0) && _paused.enQ(client))
    {
        return false;
    }

    // mark the client as joinable
    numClients = 0;
    _joinable.enQ(client);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TCPserverSocket*
NetServer::serverFind(int fd)
{
    size_t i;
    for (i = 0; i != _serverSockets.items(); i++)
    {
        auto ss = utl::cast<TCPserverSocket>(_serverSockets[i]);
        ASSERTD(ss != nullptr);
        if (ss->fd() == fd)
            return ss;
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::cleanupJoinable()
{
    while (true)
    {
        NetServerClient* client;
        if (!_joinable.deQ(client))
            break;
        _clients[client->index()].store(nullptr, std::memory_order_relaxed);
        client->join();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::cleanupPaused()
{
    while (true)
    {
        NetServerClient* client;
        if (!_paused.deQ(client))
            break;
        client->exit();
        client->wakeup();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::clientsAdd(NetServerClient* client)
{
    while (true)
    {
        auto index = _clientsIndex.fetch_add(9, std::memory_order_relaxed) & _clientsMask;
        auto& curClient = _clients[index];
        if (curClient.load(std::memory_order_relaxed) != nullptr)
        {
            continue;
        }
        NetServerClient* expected = nullptr;
        if (curClient.compare_exchange_strong(expected, client, std::memory_order_relaxed,
                                              std::memory_order_relaxed))
        {
            client->setIndex(index);
            break;
        }
        else
        {
            expected = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NetServerClient ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

NetServerClient::NetServerClient(NetServer* server, FDstream* socket, const InetHostAddress& addr)
{
    _server = server;
    _socket = socket;
    _addr = addr;
    _socketFD = socket->fd();
    _verified = false;
    _exit = false;
    _index = size_t_max;
    _numClients = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Object&
NetServerClient::getKey() const
{
    return _socketFD;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServerClient::listen()
{
    super::testCancel();

    // one millisecond seems to be enough to avoid hammering CPU
    // note: 1000 usec   = 1 ms
    //       100000 usec = 100 ms (1/10 sec)
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    // wait for an event on the socket
    int fd = _socketFD.get();
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    UTL_EINTR_LOOP(select(fd + 1, &rset, nullptr, nullptr, &tv));

    // nothing happened?
    if (err == 0)
        return;

    // error?
    if (err < 0)
        utl::errToEx();

    // read the command from the client (and respond to it)
    _server->clientReadMsg(this);
    _socket->flush();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
NetServerClient::run(void* arg)
{
    while (true)
    {
        // for the thread's first client, this never blocks (_sem starts @ 1)
        _sem.P();

        // in case we got woken up by clientDisconnectAll()
        if (_exit)
            _numClients = size_t_max;

        // handle commands until there's an exception (or we're told to stop)
        while (!_exit)
        {
            try
            {
                listen();
            }
            catch (Exception&)
            {
                break;
            }
        }

        // pause the client thread, or terminate it
        if (_server->clientDeactivate(this))
        {
            break;
        }
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServerClient::pause()
{
    delete _socket;
    _socket = nullptr;
    _addr.clear();
    _socketFD.clear();
    _verified = false;
    _exit = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServerClient::setSocket(Stream* socket)
{
    _socket = socket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServerClient::resume(FDstream* socket, const InetHostAddress& addr)
{
    ASSERTD(_socket == nullptr);
    _socket = socket;
    _socketFD = socket->fd();
    _addr = addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Stream&
NetServerClient::unbufferedSocket() const
{
    ASSERTD(_socket != nullptr);
    if (_socket->isA(BufferedStream))
    {
        BufferedStream& bs = (BufferedStream&)*_socket;
        bs.flush();
        return *bs.getStream();
    }
    else
    {
        return *_socket;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
