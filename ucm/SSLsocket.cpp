#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SSLsocket.h>
#include <libutl/String.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::SSLsocket);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

SSLsocket::SSLsocket(const InetHostAddress& hostAddr, uint16_t port)
{
    init();
    open(hostAddr, port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SSLsocket::open(const InetHostAddress& hostAddr, uint16_t port)
{
    // address must be defined
    ASSERTD(!hostAddr.isNil());

    // close any currently open connection
    close();

    // create a BIO for ssl connection
    _bio = BIO_new_ssl_connect(sslContext());

    // get the SSL pointer from the BIO
    SSL* ssl;
    BIO_get_ssl(_bio, &ssl);
    ASSERTD(ssl != nullptr);

    // enable auto-retry (to simplify reading/writing logic)
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    // set the IP address and port number for the connection
    auto ip_nbo = hostAddr.get();
#ifdef UTL_ARCH_LITTLE_ENDIAN
    auto port_nbo = reverseBytes(port);
#else
    auto port_nbo = port;
#endif
    auto ap = BIO_ADDR_new();
    BIO_ADDR_rawmake(ap, AF_INET, &ip_nbo, sizeof(ip_nbo), port_nbo);
    ASSERTFNP(BIO_set_conn_address(_bio, ap));
    BIO_ADDR_free(ap);

    // make the connection
    if ((BIO_do_connect(_bio) <= 0) || (BIO_do_handshake(_bio) <= 0))
    {
        close();
        char buf[256];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, buf, 256);
        throw StreamErrorEx(String(buf));
    }

    // OK
    setName(hostAddr);
    setMode(io_rd | io_wr);
    setError(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SSLsocket::close()
{
    if (_bio == nullptr)
        return;
    BIO_free(_bio);
    _bio = nullptr;
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SSLsocket::certificateOK() const
{
    SSL* ssl;
    BIO_get_ssl(_bio, &ssl);
    return (SSL_get_verify_result(ssl) == X509_V_OK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
SSLsocket::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(isInput());

    // check maxBytes, fix minBytes
    if (maxBytes == 0)
        return 0;
    if (minBytes > maxBytes)
        minBytes = maxBytes;

    byte_t* arrayLim = array + maxBytes;
    byte_t* arrayMin = array + minBytes;

    // read into the buffer
    byte_t* cur = array;
    do
    {
        int num = BIO_read(_bio, cur, arrayLim - cur);
        ASSERTD((num > 0) || !BIO_should_retry(_bio));

        // error condition?
        if (num < 0)
        {
            throwStreamErrorEx();
        }

        // eof?
        if (num == 0)
        {
            if (cur < arrayMin)
                throwStreamEOFex();
            setEOF(true);
            break;
        }

        // read <num> bytes
        cur += num;
    } while (cur < arrayMin);
    return cur - array;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SSLsocket::write(const byte_t* array, size_t num)
{
    ASSERTD(isOutput());
    ASSERTD(num >= 0);
    const byte_t* arrayLim = array + num;
    const byte_t* cur = array;

    while (cur != arrayLim)
    {
        int num = BIO_write(_bio, cur, arrayLim - cur);
        ASSERTD((num > 0) || !BIO_should_retry(_bio));

        // problem?
        if (num <= 0)
        {
            throwStreamErrorEx();
        }

        // wrote <num> bytes
        cur += num;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
