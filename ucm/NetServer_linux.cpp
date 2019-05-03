#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_LINUX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <libutl/BufferedStream.h>
#include <libutl/HostOS.h>
#include <libutl/LogMgr.h>
#include <libutl/NetServer_linux.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
//#define DEBUG_MODULE
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NetServer //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class QueuedConnection : public utl::Object
{
    UTL_CLASS_DECL(QueuedConnection, Object);
    UTL_CLASS_NO_COPY;

public:
    QueuedConnection(FDstream* socket_, const InetHostAddress& addr_)
        : socket(socket_)
        , addr(addr_)
    {
    }

    FDstream* socket;
    InetHostAddress addr;

private:
    void
    init()
    {
        ABORT();
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::QueuedConnection);
UTL_CLASS_IMPL_ABC(utl::NetServer);
UTL_CLASS_IMPL(utl::NetServerClient);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TDeque, utl::NetServerClient);
UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::NetServerClient);
UTL_INSTANTIATE_TPL(utl::Queue, utl::NetServerClient);
UTL_INSTANTIATE_TPL(utl::TDeque, utl::QueuedConnection);
UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::QueuedConnection);
UTL_INSTANTIATE_TPL(utl::Queue, utl::QueuedConnection);

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
NetServer::addServer(ServerSocket* serverSocket)
{
    if (!_serverSockets.add(serverSocket))
        return false;
    serverSocket->setNonBlocking();
    int fd = serverSocket->fd();
    epoll_event ev;
    bzero(&ev, sizeof(epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    ASSERTFNZ(epoll_ctl(_epi, EPOLL_CTL_ADD, fd, &ev));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::listen()
{
    super::testCancel();

    epoll_event events[2];
    UTL_EINTR_LOOP(epoll_wait(_epi, events, 2, 200));
    ASSERTD(err >= 0);
    int numEvents = err;

    // handle the events
    for (int i = 0; i != numEvents; ++i)
    {
        epoll_event& ev = events[i];
        int fd = ev.data.fd;
        if (fd == _pipe[0])
        {
            handlePipeEvent();
        }
        else
        {
            handleServerSocketEvent(fd);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::handlePipeEvent()
{
    NetServerClient* threads[8];
    UTL_EINTR_LOOP(read(_pipe[0], threads, 8 * sizeof(NetServerClient*)));
    ASSERTD(err >= 0);
    size_t numBytes = err;
    ASSERTD(numBytes >= sizeof(NetServerClient*));
    ASSERTD((numBytes & (sizeof(NetServerClient*) - 1)) == 0);
    size_t numThreads = numBytes / sizeof(NetServerClient*);
    for (size_t i = 0; i != numThreads; ++i)
    {
        auto thread = threads[i];
        if (thread->_numClients == size_t_max)
        {
            thread->_numClients = 0;
        }
        else
        {
            --_clientsCount;
        }
        if (thread->_numClients == 0)
        {
            _clients[thread->index()].store(nullptr, std::memory_order_relaxed);
            thread->join();
            thread = nullptr;
        }
        if (!_queuedConnections.empty())
        {
            if ((thread == nullptr) && !_queuedThreads.empty())
            {
                thread = _queuedThreads.deQ();
            }
            auto conn = _queuedConnections.deQ();
            clientActivate(conn->socket, conn->addr, thread);
            delete conn;
        }
        else if (thread != nullptr)
        {
            if (_queuedThreads.size() < _pausedMax)
            {
                _queuedThreads.enQ(thread);
            }
            else
            {
                thread->exit();
                thread->wakeup();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::handleServerSocketEvent(int fd)
{
    // accept the incoming socket connection

    auto ss = serverFind(fd);
    ASSERTD(ss != nullptr);
    auto clientSocket = ss->makeSocket();
    InetHostAddress clientAddr;
    if (!ss->accept(clientSocket, clientAddr))
    {
        delete clientSocket;
        return;
    }

    // too many clients already running?
    if (_clientsCount >= _clientsMax)
    {
        // queue the connected socket for later
        _queuedConnections.enQ(new QueuedConnection(clientSocket, clientAddr));
    }
    else
    {
        // start serving the client in a thread
        NetServerClient* thread;
        if (_queuedThreads.empty())
        {
            thread = nullptr;
        }
        else
        {
            thread = _queuedThreads.deQ();
        }
        clientActivate(clientSocket, clientAddr, thread);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::clientDisconnectAll()
{
    // tell the active client threads to exit
    for (size_t i = 0; i != _clientsSize; ++i)
    {
        auto& ptr = _clients[i];
        auto client = ptr.load(std::memory_order_relaxed);
        if (client != nullptr)
        {
            client->exit();
        }
    }

    // close all queued connections
    while (!_queuedConnections.empty())
    {
        auto conn = _queuedConnections.deQ();
        delete conn->socket;
        delete conn;
    }

    // join on queued threads
    while (!_queuedThreads.empty())
    {
        auto thread = _queuedThreads.deQ();
        thread->wakeup();
    }

    // process events on the pipe until no client threads exist
    if (_epi >= 0)
    {
        epiInit();
        while (true)
        {
            bool anyClient = false;
            for (size_t i = 0; i != _clientsSize; ++i)
            {
                auto& ptr = _clients[i];
                if (ptr.load(std::memory_order_relaxed) != nullptr)
                {
                    anyClient = true;
                    break;
                }
            }
            if (!anyClient)
                break;
            listen();
        }
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
    _serverSockets.setKeepSorted(true);
    _pausedMax = maxPaused;
    _clientsPerThread = clientsPerThread;
    _queuedThreads.setOwner(false);
    _queuedConnections.setOwner(false);

    // _clients
    _clientsSize = 1;
    while (_clientsSize < maxClients)
        _clientsSize <<= 1;
    _clientsSize <<= 1;
    _clientsMask = _clientsSize - 1;
    _clientsCount = 0;
    _clientsMax = maxClients;
    _clientsIndex.store(0, std::memory_order_relaxed);
    size_t clientsSizeBytes =
        utl::max((size_t)UTL_ARCH_CACHE_LINE_SIZE, _clientsSize * sizeof(client_ptr_t));
    posix_memalign((void**)&_clients, UTL_ARCH_CACHE_LINE_SIZE, clientsSizeBytes);
    auto lim = _clients + _clientsSize;
    for (auto it = _clients; it != lim; ++it)
    {
        new (it) client_ptr_t(nullptr);
    }

    _pipe[0] = _pipe[1] = -1;
    ASSERTFNZ(pipe(_pipe));
    _epi = -1;
    epiInit();
}
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::deInit()
{
    clientDisconnectAll();
    if (_pipe[0] >= 0)
        close(_pipe[0]);
    if (_pipe[1] >= 0)
        close(_pipe[1]);
    if (_epi >= 0)
        close(_epi);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::epiInit()
{
    if (_epi >= 0)
        close(_epi);
    _epi = epoll_create1(0);
    ASSERTD(_epi >= 0);
    epoll_event ev;
    bzero(&ev, sizeof(epoll_event));
    ev.events = EPOLLIN;
    ev.data.fd = _pipe[0];
    ASSERTFNZ(epoll_ctl(_epi, EPOLL_CTL_ADD, _pipe[0], &ev));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetServer::clientActivate(FDstream* socket,
                          const InetHostAddress& clientAddr,
                          NetServerClient* thread)
{
    // make or re-use a thread for the client
    if (thread == nullptr)
    {
        thread = clientMake(socket, clientAddr);
        thread->setNumClients(_clientsPerThread);
        clientsAdd(thread);
        onClientConnect(thread);
        thread->start(nullptr, true);
    }
    else
    {
        thread->resume(socket, clientAddr);
        onClientConnect(thread);
        thread->wakeup();
    }
    ++_clientsCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
NetServer::clientDeactivate(NetServerClient* client)
{
    bool res;
    auto& numClients = client->_numClients;

    // paused client is now becoming joinable?
    if (numClients == size_t_max)
    {
        res = true;
    }
    else
    {
        res = (--numClients == 0);
        onClientDisconnect(client);
        client->pause();
    }

    // write thread's address to the pipe
    UTL_EINTR_LOOP(write(_pipe[1], &client, sizeof(NetServerClient*)));

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ServerSocket*
NetServer::serverFind(int fd)
{
    size_t i;
    for (i = 0; i != _serverSockets.items(); ++i)
    {
        auto ss = utl::cast<ServerSocket>(_serverSockets[i]);
        ASSERTD(ss != nullptr);
        if (ss->fd() == fd)
            return ss;
    }
    return nullptr;
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
        // for the thread's first client, this never blocks (_sem starts at 1)
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
        auto& bs = utl::cast<BufferedStream>(*_socket);
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

#endif // UTL_HOST_OS == UTL_OS_LINUX
