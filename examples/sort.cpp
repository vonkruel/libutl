#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Float.h>
#include <libutl/List.h>
#include <libutl/OStimer.h>
#include <libutl/RandUtils.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

const size_t numItems = 2000000;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    size_t i;
    randutils::mt19937_64_rng rng;

    cout << "initializing..." << endl;
    Array unsorted;
    for (i = 0; i != numItems; ++i)
    {
        unsorted += new Uint(rng.uniform(size_t_min, size_t_max));
    }
    List list(false);
    OStimer timer;

    cout << "linked list merge-sort" << endl;
    list = unsorted;
    timer.start();
    list.sort();
    timer.stop();
    ASSERTD(list.testSorted());
    cout << "    user time = " << Float(timer.userTime()).toString(2) << " sec." << endl;

    cout << "linked list quick-sort" << endl;
    list = unsorted;
    timer.start();
    list.sort(sort_quickSort);
    timer.stop();
    ASSERTD(list.testSorted());
    cout << "    user time = " << Float(timer.userTime()).toString(2) << " sec." << endl;

    cout << "Array std::sort" << endl;
    Array array(false);
    array = unsorted;
    timer.start();
    array.sort();
    timer.stop();
    ASSERTD(array.testSorted());
    cout << "    user time = " << Float(timer.userTime()).toString(2) << " sec." << endl;

    return 0;
}
