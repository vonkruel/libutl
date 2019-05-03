#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
    struct ssl_ctx_st;
    typedef struct ssl_ctx_st SSL_CTX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Allocator;
class Object;
class Stream;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Comparison operators.
   \ingroup utility
*/
enum cmp_t
{
    cmp_lessThan,            /**< less-than */
    cmp_lessThanOrEqualTo,   /**< less-than-or-equal-to */
    cmp_equalTo,             /**< equal-to */
    cmp_greaterThan,         /**< greater-than */
    cmp_greaterThanOrEqualTo /**< greater-than-or-equal-to */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Find strategy.
   \ingroup utility
*/
enum find_t
{
    find_any = 0,   /**< find any matching object */
    find_first = 1, /**< find first matching object */
    find_last = 2,  /**< find last matching object */
    find_ip = 3     /**< find insertion point */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   I/O flags.
   \ingroup utility
*/
enum io_t
{
    io_null = 0, /**< null value */
    io_rd = 1,   /**< read */
    io_wr = 2,   /**< write */
    io_rdwr = 3  /**< read/write */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Specifies a serialization mode.
   \ingroup utility
   \see Object::serialize
*/
enum serialize_t
{
    ser_readable = 1,    /**< human-readable and portable representation */
    ser_compact = 2,     /**< space-efficient and portable representation */
    ser_nonportable = 4, /**< nonportable space- and time-efficient representation */
    ser_default = 8      /**< default representation (via getSerializeMode()) */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Initialize UTL++.  This function is called by utl::Application's constructor, so when your
   application class inherits from Application (as recommended) you won't need to worry about
   calling this function.

   \ingroup utility
*/
void init();

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   De-initialize UTL++.  This function is called by utl::Application's destructor, so when your
   application class inherits from Application (as recommended) you won't need to worry about
   calling this function.

   \ingroup utility
*/
void deInit();

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether UTL++ initialization has been done.
*/
bool initialized();

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abort the process.

   \ingroup utility
   \param filename filename where abortion occurred
   \param lineNo line number where abortion occurred
   \param text message explaining the reason for the failure
*/
void abort(const char* filename, int lineNo, const char* text = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abort the process.

   \ingroup utility
   \param text message explaining the reason for the failure
*/
void abort(const char* text);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two boolean values.

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side boolean value
   \param rhs right-hand-side boolean value
*/
int compare(bool lhs, bool rhs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Exit the process with an error code.

   \ingroup utility
   \param filename filename where failure occurred
   \param lineNo line number where failure occurred
   \param text message explaining the reason for the failure
   \param exitCode exit code
*/
void die(const char* filename, int lineNo, const char* text = nullptr, int exitCode = 1);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Exit the process with an error code.

   \ingroup utility
   \param text message explaining the reason for the failure
   \param exitCode exit code
*/
void die(const char* text, int exitCode = 1);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Get the default serialization mode (see utl::serialize_t).

   \see Object::serialize
   \ingroup utility
*/
uint_t getSerializeMode();

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Set the default serialization mode (see utl::serialize_t).

   \see Object::serialize
   \ingroup utility
*/
void setSerializeMode(uint_t mode);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Re-allocate the given block.

   \ingroup utility
   \return address of re-allocated block
   \param ptr address of block to re-allocate
   \param size current size of block
   \param newSize new size of block
*/
void* realloc(void* ptr, size_t size, size_t newSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Duplicate the given string.

   \ingroup utility
   \return address of newly allocated block
   \param str string to duplicate
*/
char* strdup(const char* str);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Get a pointer to the global SSL context.

   \ingroup utility
   \return global SSL_CTX*
*/
SSL_CTX* sslContext();

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Set default locations for trusted CA certificates.

   \ingroup utility
   \return true iff the operation succeeded
   \param caFile path to a file of CA certificates in PEM format
   \param caPath path to a directory containing CA certificates in PEM format
*/
bool sslVerifyLocations(const char* caFile, const char* caPath);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization of Built-In Types /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a boolean.

   \ingroup utility
   \param val boolean to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(bool& val, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an 8-bit integer.

   \ingroup utility
   \param val integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(int8_t& val, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a character.

   \ingroup utility
   \param val character to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(char& val, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an 8-bit unsigned character.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(uint8_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a 16-bit integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(int16_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an unsigned 16-bit integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(uint16_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a 32-bit integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(int32_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an unsigned 32-bit integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(uint32_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a long integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(long& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an unsigned long integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(ulong_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_SIZEOF_LONG == 4

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an signed 64-bit integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(int64_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an unsigned long integer.

   \ingroup utility
   \param i integer to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(uint64_t& i, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UTL_SIZEOF_LONG == 4

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a double-precision floating point number.

   \ingroup utility
   \param n double to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(double& n, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an object.

   \ingroup utility
   \param object object to serialize to/from stream
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void serialize(Object& object, Stream& stream, uint_t io, uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

// disallow serialization of pointer types
UTL_TYPE_NO_SERIALIZE(void*);
UTL_TYPE_NO_SERIALIZE(char*);
UTL_TYPE_NO_SERIALIZE(byte_t*);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
