#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include "Card.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class Hand : public Object, protected FlagsMI
{
    UTL_CLASS_DECL(Hand, Object);

public:
    virtual int compare(const Object& rhs) const;

    virtual String toString() const;

    void
    add(const Card* card)
    {
        _cards += card;
    }

    void
    clear()
    {
        _cards.clear();
    }

    uint_t
    numCards() const
    {
        return _cards.items();
    }

    const TArray<Card>&
    getCards() const
    {
        return _cards;
    }

    Hand&
    operator+=(const Card* card)
    {
        add(card);
        return *this;
    }

    bool isBlackjack() const;

    bool
    isBust() const
    {
        return (value() > 21);
    }

    bool
    isDealer() const
    {
        return getFlag(flg_dealer);
    }

    void
    setDealer(bool dealer)
    {
        setFlag(flg_dealer, dealer);
    }

    bool
    isHard() const
    {
        return !isSoft();
    }

    bool
    isSoft() const
    {
        bool res;
        value(&res);
        return res;
    }

    const String&
    getName() const
    {
        return _name;
    }

    void
    setName(const String& name)
    {
        _name = name;
    }

    bool
    isHittable() const
    {
        return !(isSplitAce() && (numCards() == 2));
    }

    bool
    isSplit() const
    {
        return getFlag(flg_split);
    }

    bool isSplitAce() const;

    void
    setSplit(bool split)
    {
        setFlag(flg_split, split);
    }

    bool isSplittable() const;

    void
    setSplittable(bool splittable)
    {
        setFlag(flg_splittable, splittable);
    }

    void split(Hand& rhs);

    uint_t value(bool* soft = nullptr) const;

private:
    void
    init()
    {
        _cards.setOwner(false);
        setSplittable(true);
    }
    void
    deInit()
    {
    }

private:
    enum flg_t
    {
        flg_dealer,
        flg_split,
        flg_splittable
    };

private:
    TArray<Card> _cards;
    String _name;
};
