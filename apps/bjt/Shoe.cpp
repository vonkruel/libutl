#include "common.h"
#include "Shoe.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(Shoe);

////////////////////////////////////////////////////////////////////////////////////////////////////

const Card*
Shoe::draw()
{
    ASSERTD(!_cards.empty());
    const Card* card = _cards.pop();
    _usedCards += card;
    return card;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Shoe::setDecks(uint_t num, uint_t shuffleIterations)
{
    clear();
    uint_t i;
    for (i = 0; i != num; i++)
    {
        uint_t face;
        for (face = 0; face != cf_num_faces; face++)
        {
            uint_t suit;
            for (suit = 0; suit != cs_num_suits; suit++)
            {
                _cards += new Card((card_face_t)face, (card_suit_t)suit);
            }
        }
    }
    // the last ~20% of cards are never dealt (to manage exposure to card-counting)
    _reserve = (_cards.items() * 20) / 100;
    _shuffleIterations = shuffleIterations;
    shuffle();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Shoe::shuffle()
{
    _cards.setOwner(false);
    _usedCards.setOwner(false);
    _cards += _usedCards;
    _usedCards.clear();
    _cards.setOwner(true);
    _usedCards.setOwner(true);
    for (uint_t i = 0; i != _shuffleIterations; i++)
    {
        _cards.shuffle(0, _cards.items(), &_rng);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Shoe::init()
{
    _reserve = 0;
    _shuffleIterations = 4;
}
