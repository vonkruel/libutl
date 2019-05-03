#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/MemStream.h>
#include <libutl/RBtree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE
UTL_APP(Test)
UTL_MAIN_RL(Test)

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Customer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

String
customerToString(Customer customer)
{
    return customer.toString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    // create some Customer objects
    Customer custA(0, "Al Franken");
    Customer custB(1, "Bob Jones");

    //
    // String representations
    //
    cout << "--- String representations" << endl;
    cout << "custA = " << custA << endl;
    cout << "custB = " << custB << endl;
    cout << endl;

    //
    // Comparisons
    //
    cout << "--- comparisons" << endl;
    // custA is equal to itself
    cout << "custA == custA   : " << Bool(custA == custA) << endl;
    // custA and custB are not equal
    cout << "custA == custB   : " << Bool(custA == custB) << endl;
    cout << "custA != custB   : " << Bool(custA != custB) << endl;
    // custA < custB because it has lesser ID
    cout << "custA <  custB   : " << Bool(custA < custB) << endl;
    // custB > custA because it has a greater ID
    cout << "custB >  custA   : " << Bool(custB > custA) << endl;
    // custA == Uint(0) because its key (id) is 0
    cout << "custA == Uint(0) : " << Bool(custA == Uint(0)) << endl;
    // custA < Uint(1) because its key (id) is < 1
    cout << "custA <  Uint(1) : " << Bool(custA < Uint(1)) << endl;
    // custB > Uint(0) because its key (id) is > 0
    cout << "custB >  Uint(0) : " << Bool(custB > Uint(0)) << endl;
    // custB == Uint(1) because its key (id) is 1
    cout << "custB == Uint(1) : " << Bool(custB == Uint(1)) << endl;
    // custB < Uint(2) because its key (id) is < 2
    cout << "custB <  Uint(2) : " << Bool(custB < Uint(2)) << endl;
    cout << endl;

    //
    // Copying
    //
    cout << "--- copying" << endl;
    Customer cust;
    // assignment operator (operator=)
    cust = custA;
    cout << "assignment operator: copy of custA: " << cust << endl;
    // clone()
    Customer* custPtr = custB.clone();
    cout << "clone(): copy of custB: " << *custPtr << endl;
    delete custPtr;
    // copy constructor
    cout << "copy constructor: copy of custA: " << customerToString(custA) << endl;
    cout << endl;

    //
    // Conversion to pointer
    //
    cout << "--- conversion to pointer" << endl;
    const Customer* custAptr = custA;
    const Customer* custBptr = custB;
    cout << "custAptr = " << (void*)custAptr << endl;
    cout << "custBptr = " << (void*)custBptr << endl;
    cout << endl;

    //
    // RTTI
    //
    cout << "--- RTTI system" << endl;
    cout << "custA class: " << custA.getClassName() << endl;
    cout << "custA's base class: " << custA.getBaseClassName() << endl;
    cout << "custA.isA(Object) = " << Bool(custA.isA(Object)) << endl;
    cout << "custA.isA(Uint) = " << Bool(custA.isA(Uint)) << endl;
    cout << endl;

    //
    // Hash codes
    //
    cout << "--- hash codes" << endl;
    // If an object has a key, its hash code is the hash code for its
    // key (unless the object's class overrides Object::hash())
    cout << "custA.hash() = " << custA.hash(size_t_max) << endl;
    cout << "custB.hash() = " << custB.hash(size_t_max) << endl;
    cout << endl;

    //
    // Serialization
    //
    cout << "--- serialization" << endl;
    // create an array and serialize it out to a memory buffer
    cout << "serializing array to memStream . . ." << endl;
    Array myArray(false);
    myArray += custA;
    myArray += custB;
    MemStream memStream;
    myArray.serializeOut(memStream);
    // create a red/black tree and serialize the objects into it
    cout << "serializing red/black tree from memStream . . ." << endl;
    RBtree myTree;
    myTree.serializeIn(memStream);
    myTree.dumpWithClassName(cout);
    cout << endl;

    return 0;
}
