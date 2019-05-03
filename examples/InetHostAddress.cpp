#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/InetHostAddress.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    // get local host FQDN
    InetHostname localFQDN = InetHostname::localFQDN();
    cout << "local host FQDN = " << localFQDN << endl;

    // find addresses for FQDN
    Array ips;
    localFQDN.addresses(ips);
    cout << "local host address(es) = " << ips.toString(", ") << endl;
    return 0;
}
