#include "common.h"
#include "Hand.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(Hand);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Hand::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(Hand))
    {
        const Hand& hand = (const Hand&)rhs;
        uint_t lhsValue = value();
        uint_t rhsValue = hand.value();
        if (!isDealer() && isBlackjack())
        {
            lhsValue++;
        }
        else if (lhsValue > 21)
        {
            lhsValue = 0;
        }
        if (!hand.isDealer() && hand.isBlackjack())
        {
            rhsValue++;
        }
        else if (rhsValue > 21)
        {
            rhsValue = 0;
        }
        res = utl::compare(lhsValue, rhsValue);
    }
    else
    {
        res = Object::compare(rhs);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Hand::toString() const
{
    String str = _name;
    str += ": ";
    uint_t i, i_numCards = _cards.items();
    for (i = 0; i < i_numCards; i++)
    {
        if (i > 0)
            str += ", ";
        str += _cards[i]->faceString();
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hand::isBlackjack() const
{
    if (_cards.items() == 2)
    {
        const Card* firstCard = (const Card*)_cards.first();
        const Card* secondCard = (const Card*)_cards.last();
        if (((firstCard->faceValue() == 10) && (secondCard->faceValue() == 11)) ||
            ((secondCard->faceValue() == 10) && (firstCard->faceValue() == 11)))
        {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hand::isSplitAce() const
{
    if (isSplit())
    {
        const Card* card = (const Card*)_cards.first();
        return (card->face() == cf_ace);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hand::isSplittable() const
{
    if (getFlag(flg_splittable) && _cards.items() == 2)
    {
        const Card* firstCard = (const Card*)_cards.first();
        const Card* secondCard = (const Card*)_cards.last();
        return (firstCard->face() == secondCard->face());
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hand::split(Hand& hand)
{
    ASSERTD(isSplittable());
    hand += _cards[1];
    _cards.remove(1);

    // mark both hands as being split-derived
    setSplit(true);
    hand.setSplit(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
Hand::value(bool* soft) const
{
    uint_t val = 0;

    if (soft != nullptr)
    {
        *soft = false;
    }

    // add up values of all cards
    bool hasAce = false;
    uint_t i, i_numCards = _cards.items();
    for (i = 0; i < i_numCards; i++)
    {
        val += _cards[i]->faceValue();
        if (_cards[i]->face() == cf_ace)
        {
            hasAce = true;
            val -= 10;
        }
    }

    if (hasAce && ((val + 10) <= 21))
    {
        if (soft != nullptr)
        {
            *soft = true;
        }
        val += 10;
    }

    return val;
}
