#include <libutl/libutl.h>
#include <libutl/Application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Application);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Application::init()
{
    utl::init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Application::deInit()
{
    utl::deInit();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
