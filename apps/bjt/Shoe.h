#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RandUtils.h>
#include <libutl/Stack.h>
#include "Card.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class Shoe : public Object
{
    UTL_CLASS_DECL(Shoe, Object);

public:
    Shoe(uint_t numDecks, uint_t shuffleIterations = 4)
    {
        init();
        setDecks(numDecks, shuffleIterations);
    }

    void
    clear()
    {
        _cards.clear();
        _usedCards.clear();
        _reserve = 0;
    }

    const Card* draw();

    void setDecks(uint_t num, uint_t shuffleIterations = 4);

    void shuffle();

    bool
    shuffleTime() const
    {
        return (_cards.items() < _reserve);
    }

private:
    void init();
    void
    deInit()
    {
    }

private:
    Stack<Card> _cards;
    Array _usedCards;
    uint_t _reserve;
    uint_t _shuffleIterations;
    randutils::mt19937_32_rng _rng;
};
