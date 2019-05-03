#include "common.h"
#include "Card.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(Card);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Card::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(Card))
    {
        const Card& card = (const Card&)rhs;
        res = utl::compare((uint_t)_face, (uint_t)card._face);
        if (res != 0)
            return res;
        res = utl::compare((uint_t)_suit, (uint_t)card._suit);
    }
    else
    {
        res = Object::compare(rhs);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Card::faceStrings[cf_num_faces] = {"2", "3",  "4", "5", "6", "7", "8",
                                               "9", "10", "J", "Q", "K", "A"};

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t Card::faceValues[cf_num_faces] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};
