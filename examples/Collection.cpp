#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Hashtable.h>
#include <libutl/OStimer.h>
#include <libutl/RBtree.h>
#include <libutl/SkipList.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

void
testCollection(Collection* col, size_t numItems = 1000)
{
    cout << "testing " << col->getClassName() << "..." << endl;

    size_t i;

    // add items
    for (i = 0; i < numItems; i++)
    {
        col->add(new Uint(i));
    }

    // make sure we can find all items
    for (i = 0; i < numItems; i++)
    {
        ASSERT(col->find(Uint(i)) != nullptr);
    }

    // iterate through items
    BitArray found(numItems);
    for (auto uint_ : *col)
    {
        Uint& uint = utl::cast<Uint>(*uint_);
        ASSERT(uint < numItems);
        found[(size_t)uint] = true;
    }

    // make sure we found everything
    for (i = 0; i < numItems; i++)
    {
        ASSERT(found[i] == true);
    }

    // iterate through, removing every 10th object
    Collection::iterator it = col->begin();
    size_t idx = 0;
    while (*it != nullptr)
    {
        if ((++idx % 10) == 0)
        {
            ++it;
            Object* nextObject = *it;
            --it;

            size_t num = utl::cast<Uint>(**it);
            found[num] = false;
            col->removeIt(it);

            ASSERT(*it == nextObject);

            // make sure we found everything
            for (i = 0; i < numItems; i++)
            {
                bool foundI = (col->find(Uint(i)) != nullptr);
                ASSERT(found[i] == foundI);
            }
        }
        else
        {
            ++it;
        }
    }

    // make sure that objects we removed are actually gone
    // and objects we didn't remove are still there
    // iterate through items
    BitArray found2(numItems);
    for (auto uint_ : *col)
    {
        Uint& uint = utl::cast<Uint>(*uint_);
        ASSERT(uint < numItems);
        found2[(size_t)uint] = true;
    }
    for (i = 0; i < numItems; i++)
    {
        ASSERT(found[i] == found2[i]);
    }

    delete col;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
testCollectionPerformance(Collection* col, size_t numItems = 500000)
{
    cout << "performance testing " << col->getClassName() << " (numItems = " << numItems << ")..."
         << endl;

    // create an array of numbers in random order
    Array array;
    size_t i;
    for (i = 0; i < numItems; i++)
    {
        array += new Uint(i);
    }
    array.shuffle();

    // add objects to collection
    OStimer timer;
    timer.start();
    col->setOwner(false);
    for (i = 0; i < numItems; i++)
    {
        col->add(array[i]);
    }
    if (col->isA(Array))
    {
        auto& array = utl::cast<Array>(*col);
        if (!array.isKeptSorted())
        {
            array.sort();
            array.setKeepSorted(true);
        }
    }
    timer.stop();
    cout << "add objects:     " << timer.userTime() << " sec." << endl;

    // find objects in collection
    timer.start();
    col->setOwner(false);
    for (i = 0; i < numItems; i++)
    {
        ASSERT(col->find(array(i)) != nullptr);
    }
    timer.stop();
    cout << "find objects:    " << timer.userTime() << " sec." << endl;

    // iterate through objects in collection
    timer.start();
    BidIt* it = col->beginNew();
    while (**it != nullptr)
    {
        it->forward();
    }
    delete it;
    timer.stop();
    cout << "iterate objects: " << timer.userTime() << " sec." << endl;

    // remove objects from collection
    timer.start();
    for (i = 0; i < numItems; i++)
    {
        ASSERT(col->remove(array(i)));
    }
    timer.stop();
    cout << "remove objects:  " << timer.userTime() << " sec." << endl;

    delete col;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    testCollection(new Array);
    testCollection(new Hashtable);
    testCollection(new RBtree);
    testCollection(new SkipList);

    testCollectionPerformance(new Array(true, true, false));
    testCollectionPerformance(new Hashtable);
    testCollectionPerformance(new RBtree);
    testCollectionPerformance(new SkipList(true, false, nullptr, 18));

    return 0;
}
