#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Decimal.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Decimal decMin(int64_t_min), decMax(int64_t_max);
    cout << "min decimal value: " << decMin << endl;
    cout << "max decimal value: " << decMax << endl;
    Decimal a("99.999999");
    Decimal b("12.001491");
    cout << "a = " << a << endl;
    cout << "a.toString(2) = " << a.toString(2) << endl;
    cout << "b = " << b << endl;
    cout << "a + b = " << (a + b) << endl;
    cout << "a - b = " << (a - b) << endl;
    cout << "a * b = " << (a * b) << endl;
    cout << "a / b = " << (a / b) << endl;

    Decimal c("2.000000");
    Decimal d("3.000000");
    cout << "c = " << c << endl;
    cout << "d = " << d << endl;
    cout << c << " / " << d << " = " << (c / d) << endl;

    Decimal e("-340");
    Decimal f("60");
    cout << "e = " << e << endl;
    cout << "f = " << f << endl;
    cout << "e % f = " << (e % f) << endl;
    cout << "a % f = " << (a % f) << endl;

    return 0;
}
