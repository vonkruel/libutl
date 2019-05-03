#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/win32api.h>
#include <libutl/Array.h>
#include <libutl/FDstream.h>
#include <libutl/Hashtable.h>
#include <libutl/List.h>
#include <libutl/Mutex.h>
#include <libutl/RingBuffer.h>
#include <libutl/Semaphore.h>
#include <libutl/TCPserverSocket.h>
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class NetServerClient;

////////////////////////////////////////////////////////////////////////////////////////////////////
// NetServer ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstract base for multi-threaded network server.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class NetServer : public Thread
{
    friend class NetServerClient;
    UTL_CLASS_DECL_ABC(NetServer, Thread);

public:
    /**
       Constructor.
       \param maxClients max. simultaneous clients
       \param maxPaused (optional : 1) maximum number of paused clients
       \param clientsPerThread (optional : 1) number of clients handled by each thread
    */
    NetServer(size_t maxClients, size_t maxPaused = 1, size_t clientsPerThread = 1)
    {
        init(maxClients, maxPaused, clientsPerThread);
    }

    virtual void* run(void* arg = nullptr);

    /**
       Add a server socket.
       \param serverSocket server socket
    */
    bool addServer(TCPserverSocket* serverSocket);

    /**
       Listen on server sockets for new client connections.
    */
    void listen();

    /** Disconnect all clients. */
    void clientDisconnectAll();

    /**
       Disconnect the given client.
       \param client client to be disconnected
    */
    void clientDisconnect(NetServerClient* client);

protected:
    /**
       Make a new client object.
       \param socket client socket
       \param addr client address
    */
    virtual NetServerClient* clientMake(FDstream* socket, const InetHostAddress& addr);

    /** Find a client by its file descriptor. */
    NetServerClient* clientFind(int fd) const;

    /**
       When the server is busy, write an explanatory message to the
       client before disconnecting it.
       \param os output stream to write message to
    */
    virtual void clientWriteServerBusyMsg(Stream& os) = 0;

    /**
       Notify of a new client connection.
       \param client newly connected client
    */
    virtual void onClientConnect(NetServerClient* client) = 0;

    /**
       Disconnect a client.
       \param client client to disconnect
    */
    virtual void onClientDisconnect(NetServerClient* client) = 0;

    /** Is it time for the server to stop? */
    virtual bool exiting() const;

private:
    void init(size_t maxClients = size_t_max, size_t maxPaused = 1, size_t clientsPerThread = 1);
    void deInit();

    bool clientDeactivate(NetServerClient* client);

    /**
       Read (and handle) a client command.
       \param client client to read message from
    */
    virtual void clientReadMsg(NetServerClient* client) = 0;

    TCPserverSocket* serverFind(int fd);

    void cleanupJoinable();

    void cleanupPaused();

    void clientsAdd(NetServerClient* client);

    bool pausedAdd(NetServerClient* client);

    NetServerClient* pausedFind();

private:
    using client_ptr_t = std::atomic<NetServerClient*>;

private:
    Array _serverSockets;
    fd_set _serverFDs;
    int _maxFD;
    size_t _clientsPerThread;

    // all clients
    size_t _clientsSize;
    size_t _clientsMask;
    size_t _clientsMax;
    client_ptr_t* _clients;

    // shared counters
    char pad0[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _clientsCount;
    char pad1[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _clientsIndex;

    // paused clients
    utl::RingBuffer<NetServerClient*> _paused;

    // joinable clients
    utl::RingBuffer<NetServerClient*> _joinable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// NetServerClient /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Client connection to NetServer.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class NetServerClient : public Thread
{
    friend class NetServer;
    UTL_CLASS_DECL(NetServerClient, Thread);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    NetServerClient(NetServer* server, FDstream* socket, const InetHostAddress& addr);

    virtual const Object& getKey() const;

    void listen();

    virtual void* run(void* arg = nullptr);

    virtual void pause();

    virtual void resume(FDstream* socket, const InetHostAddress& addr);

    inline void
    wakeup()
    {
        _sem.V();
    }

    void setSocket(Stream* stream);

    Stream&
    socket() const
    {
        ASSERTD(_socket != nullptr);
        return *_socket;
    }

    Stream& unbufferedSocket() const;

    const InetHostAddress&
    addr() const
    {
        return _addr;
    }

    void
    setAddr(const InetHostAddress& addr)
    {
        _addr = addr;
    }

    bool
    isVerified() const
    {
        return _verified;
    }

    void
    setVerified(bool verified)
    {
        _verified = verified;
    }

    void
    exit()
    {
        _exit = true;
    }

    size_t
    index() const
    {
        return _index;
    }

    void
    setIndex(size_t index)
    {
        _index = index;
    }

    void
    setNumClients(size_t numClients)
    {
        _numClients = numClients;
    }

    bool
    isPaused() const
    {
        return (_numClients > 0) && (_socket == nullptr);
    }

    bool
    isJoinable() const
    {
        return (_numClients == 0);
    }

private:
    void
    init()
    {
        ABORT();
    }
    void
    deInit()
    {
        delete _socket;
    }

private:
    NetServer* _server;
    Stream* _socket;
    InetHostAddress _addr;
    Uint _socketFD;
    bool _verified;
    bool _exit;
    size_t _index;
    size_t _numClients;
    utl::Semaphore _sem;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
