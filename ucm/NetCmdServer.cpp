#include <libutl/libutl.h>
#include <libutl/LogMgr.h>
#include <libutl/NetCmdServer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::NetCmdServer);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
NetCmdServer::clientReadMsg(NetServerClient* client)
{
    SCOPE_FAIL
    {
        clientDisconnect(client);
    };
    Array cmd;
    cmd.serializeIn(client->socket());
    if (cmd.empty())
    {
        throw StreamSerializeEx();
    }
    handleCmd(client, cmd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;