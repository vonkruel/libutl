#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum card_face_t
{
    cf_two,
    cf_three,
    cf_four,
    cf_five,
    cf_six,
    cf_seven,
    cf_eight,
    cf_nine,
    cf_ten,
    cf_jack,
    cf_queen,
    cf_king,
    cf_ace,
    cf_num_faces
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum card_suit_t
{
    cs_clubs,
    cs_diamonds,
    cs_hearts,
    cs_spades,
    cs_num_suits
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Card : public Object
{
    UTL_CLASS_DECL(Card, Object);

public:
    Card(card_face_t face, card_suit_t suit)
    {
        _face = face;
        _suit = suit;
    }

    virtual int compare(const Object& rhs) const;

    card_face_t
    face() const
    {
        return _face;
    }

    uint_t
    faceIdx() const
    {
        return (uint_t)_face;
    }

    String
    faceString() const
    {
        return String(faceStrings[faceIdx()], false);
    }

    uint_t
    faceValue() const
    {
        return faceValues[faceIdx()];
    }

    uint_t
    faceValueIdx() const
    {
        return (faceValue() - 2);
    }

    card_suit_t
    suit() const
    {
        return _suit;
    }

private:
    void
    init()
    {
        ABORT();
    }
    void
    deInit()
    {
    }

private:
    card_face_t _face;
    card_suit_t _suit;
    static const char* faceStrings[cf_num_faces];
    static const uint_t faceValues[cf_num_faces];
};
