#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/NamedObjectMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Counter for resources shared between threads.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Semaphore : public Object, public NamedObjectMI
{
    UTL_CLASS_DECL(Semaphore, Object);
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor.
       \param count initial counter value
    */
    Semaphore(uint_t count)
    {
        init(count);
    }

    /**
       Try to decrement the count (without blocking).
       \return true if count decremented, false otherwise
    */
    bool trydown();

    /** Block until count is non-zero and decrement count. */
    void down();

    /** Increment the count. */
    void up();

    /** Alias for trydown(). */
    void
    tryP()
    {
        trydown();
    }

    /** Alias for down(). */
    void
    P()
    {
        down();
    }

    /** Alias for up(). */
    void
    V()
    {
        up();
    }

private:
    void init(uint_t count = 1);
    void deInit();

private:
#if UTL_HOST_TYPE == UTL_HT_UNIX
    sem_t _sem;
#else
    HANDLE _sem;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
