#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Application.h>
#include "Hand.h"
#include "Shoe.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_EXCEPTION_DECL(QuitEx, utl::Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////

class BlackjackTrainer : public Application
{
    UTL_CLASS_DECL(BlackjackTrainer, Application);

public:
    virtual int run(int argc = 0, char** argv = nullptr);

private:
    void init();
    void deInit();

    void buyChips();
    void playHand();
    double placeBet();
    void player(const Card* dealerCard,
                Hand& playerHand,
                double& playerBet,
                bool& playerCorrect,
                bool& split);
    void hit(Hand& playerHand);
    void dealer(Hand& dealerHand);
    void assess(Hand& dealerHand, Hand& playerHand, double playerBet, bool correct);

    uint_t
    getRecommendation(const Card* dealerCard, const Hand& playerHand, double playerBet) const;
    void printRecommendation(uint_t recommendation) const;
    void printStats() const;
    static String money(double amount);
    static String percent(double pct);

    void bell();
    void backspace(uint_t num = 1);
    void destructiveBackspace(uint_t num = 1);

private:
    // game state
    Shoe _shoe;
    double _playerPrevBet;
    double _playerCash;

    // statistics
    uint_t _numHands;
    uint_t _numHandsCorrect;
    uint_t _numHandsTiedCorrect;
    uint_t _numHandsTiedIncorrect;
    uint_t _numHandsWonCorrect;
    uint_t _numHandsWonIncorrect;
    double _totalBetCorrect;
    double _totalBetIncorrect;
    double _totalWinningsCorrect;
    double _totalWinningsIncorrect;

    // static data
    static const uint_t splitTable[10][10];
    static const uint_t softTable[6][10];
    static const uint_t hardTable[8][10];
    static const char* actionStrings[5];
};
