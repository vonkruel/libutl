#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Application base class.

   Any application you write with UTL++ should contain a single class that inherits from
   Application.  Application's constructor calls utl::init(), and its destructor calls
   utl::deInit().

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Application : public Object
{
    UTL_CLASS_DECL_ABC(Application, Object);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    /**
       Run the application.
       \return application's exit code
       \param argc number of parameters (as in main())
       \param argv array of parameters (as in main())
    */
    virtual int run(int argc = 0, char** argv = nullptr) = 0;

private:
    void init();
    void deInit();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare a simple application.  It is appropriate for short, simple applications such as
   example programs.
   \ingroup general
*/
#define UTL_APP(appName)                                                                           \
    class appName : public utl::Application                                                        \
    {                                                                                              \
        UTL_CLASS_DECL(appName, utl::Application);                                                 \
        UTL_CLASS_DEFID;                                                                           \
                                                                                                   \
    public:                                                                                        \
        virtual int run(int argc = 0, char** argv = nullptr);                                      \
    };                                                                                             \
    UTL_CLASS_IMPL(appName);
