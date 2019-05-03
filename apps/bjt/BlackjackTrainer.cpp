#include "common.h"
#include <libutl/BufferedFDstream.h>
#include <libutl/HostOS.h>
#include <libutl/Float.h>
#include "BlackjackTrainer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(BlackjackTrainer);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(QuitEx);

QuitEx::QuitEx(utl::Object* object)
    : utl::Exception("Quit", object)
{
}

QuitEx::QuitEx(const utl::Object& object)
    : utl::Exception("Quit", object)
{
}

void
QuitEx::init()
{
    setName("Quit");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

enum hand_result_t
{
    hr_win,
    hr_win_blackjack,
    hr_lose,
    hr_push
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum player_action_t
{
    pa_hit,
    pa_stand,
    pa_double,
    pa_split,
    pa_double_or_hit,
    pa_double_or_stand,
    pa_none
};

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BlackjackTrainer::run(int, char**)
{
    // play with six decks
    _shoe.setDecks(6);

    // output a help message
    cout << "==========================================================\n"
            "Welcome to the Blackjack table!\n"
            "\n"
            "Game parameters are:\n"
            "    o 6 decks\n"
            "    o dealer stands on all 17s\n"
            "    o splitting allowed (once)\n"
            "    o double any 2 cards\n"
            "    o double after split allowed\n"
            "    o no insurance\n"
            "    o no surrender\n"
            "\n"
            "Keys are:\n"
            "    h  :  [h]it\n"
            "    s  :  [s]tand\n"
            "    d  :  [d]ouble down\n"
            "    p  :  s[p]lit pair\n"
            "    r  :  [r]ecommendation\n"
            "    t  :  s[t]atistics\n"
            "    q  :  [q]uit\n"
            "==========================================================\n\n";

    // buy some chips
    buyChips();

    // play blackjack . . .
    try
    {
        for (;;)
        {
            playHand();
        }
    }
    catch (QuitEx&)
    {
        printStats();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::init()
{
    // stdin and stdout must be TTYs
    ASSERT(cin.isTTY());
    ASSERT(cout.isTTY());

    // init player data
    _playerPrevBet = 0;
    _playerCash = 0;

    _numHands = 0;
    _numHandsCorrect = 0;
    _numHandsTiedCorrect = 0;
    _numHandsTiedIncorrect = 0;
    _numHandsWonCorrect = 0;
    _numHandsWonIncorrect = 0;
    _totalBetCorrect = 0;
    _totalBetIncorrect = 0;
    _totalWinningsCorrect = 0;
    _totalWinningsIncorrect = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::deInit()
{
#if UTL_HOST_OS != UTL_OS_MINGW
    hostOS->setCanonicalTTY(0, true);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::buyChips()
{
    while (_playerCash < 1.0)
    {
        cout << "Cash: $" << flush;
        String cashString;
        cin >> cashString;
        _playerCash = Float(cashString);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::playHand()
{
    // player places bet
    double playerBet[2];
    playerBet[0] = placeBet();

    // dealer gets two cards
    Hand dealerHand;
    dealerHand.setName("Dealer");
    dealerHand.setDealer(true);
    const Card* dealerShownCard = _shoe.draw();
    dealerHand += dealerShownCard;
    cout << dealerHand << endl;
    const Card* dealerHiddenCard = _shoe.draw();
    dealerHand += dealerHiddenCard;

    // player gets two cards
    Hand playerHand[2];
    bool playerCorrect[2];
    playerHand[0].setName("Player");

    playerHand[0] += _shoe.draw();
    playerHand[0] += _shoe.draw();

    // player action
    bool split;
    player(dealerShownCard, playerHand[0], playerBet[0], playerCorrect[0], split);
    if (split)
    {
        playerHand[0].split(playerHand[1]);
        playerHand[0].setName("Player<1>");
        playerHand[1].setName("Player<2>");
        playerHand[0].setSplittable(false);
        playerHand[1].setSplittable(false);
        playerBet[1] = playerBet[0];
        _playerCash -= playerBet[1];
        bool dummySplit;
        player(dealerShownCard, playerHand[0], playerBet[0], playerCorrect[1], dummySplit);
        player(dealerShownCard, playerHand[1], playerBet[1], playerCorrect[1], dummySplit);
    }

    // dealer action
    bool playerIsBust = playerHand[0].isBust();
    bool playerHasBlackjack = playerHand[0].isBlackjack();
    if (split)
    {
        playerIsBust = (playerIsBust && playerHand[1].isBust());
        playerHasBlackjack = (playerHasBlackjack && playerHand[1].isBlackjack());
    }
    if (!(playerIsBust || (playerHasBlackjack && (dealerShownCard->face() != cf_ace) &&
                           (dealerShownCard->faceValue() != 10))))
    {
        dealer(dealerHand);
    }

    // assess result
    assess(dealerHand, playerHand[0], playerBet[0], playerCorrect[0]);
    if (split)
    {
        assess(dealerHand, playerHand[1], playerBet[1], playerCorrect[1]);
    }

    if (_playerCash == 0)
    {
        throw QuitEx();
    }

    // reset dealer and player hands
    dealerHand.clear();
    playerHand[0].clear();
    playerHand[1].clear();

    cout << endl;

    // shuffle if necessary
    if (_shoe.shuffleTime())
    {
        cout << "shuffling . . ." << endl << endl;
        hostOS->sleep(1);
        _shoe.shuffle();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
BlackjackTrainer::placeBet()
{
    double playerBet;
    if (_playerPrevBet > _playerCash)
    {
        _playerPrevBet = 0;
    }
    playerBet = 0;
    while ((playerBet == 0) || (playerBet > _playerCash))
    {
        cout << "Cash: " << money(_playerCash) << ", Bet";
        if (_playerPrevBet != 0)
        {
            cout << " [" << money(_playerPrevBet) << "]";
        }
        cout << ": $" << flush;
        String betString;
        cin >> betString;
        if (tolower(betString[0]) == 'q')
        {
            throw QuitEx();
        }
        try
        {
            playerBet = Float(betString);
        }
        catch (IllegalValueEx&)
        {
            playerBet = _playerPrevBet;
        }
    }
    _playerCash -= playerBet;
    _playerPrevBet = playerBet;
    return playerBet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::player(
    const Card* dealerCard, Hand& playerHand, double& playerBet, bool& playerCorrect, bool& split)
{
    playerCorrect = true;
    split = false;

    // show player hand
    cout << playerHand << " > " << flush;

// player plays his hand
#if UTL_HOST_OS != UTL_OS_MINGW
    hostOS->setCanonicalTTY(0, false);
#endif
    bool stand = false;
    while (!stand && !split)
    {
        bool isSoft;
        uint_t val = playerHand.value(&isSoft);
        bool allowHit = ((isSoft && (val < 19)) || (!isSoft && (val < 17)));
        if (!playerHand.isSplittable())
        {
            // stand on soft 19 or hard 17
            if (!allowHit || !playerHand.isHittable())
            {
                destructiveBackspace(3);
                break;
            }
        }

        uint_t recommendation = getRecommendation(dealerCard, playerHand, playerBet);

        uint_t action = pa_none;

        char c;
        cin >> c;
        c = tolower(c);
        switch (c)
        {
        case 'd':
            if ((playerHand.numCards() == 2) && (_playerCash >= playerBet))
            {
                action = pa_double;
            }
            else
            {
                bell();
            }
            break;
        case 'h':
            if (allowHit)
            {
                action = pa_hit;
            }
            else
            {
                bell();
            }
            break;
        case 'p':
            if (playerHand.isSplittable() && (_playerCash >= playerBet))
            {
                action = pa_split;
            }
            else
            {
                bell();
            }
            break;
        case 'q':
            destructiveBackspace(3);
            cout << endl;
            throw QuitEx();
            break;
        case 'r':
            destructiveBackspace(3);
            printRecommendation(recommendation);
            cout << playerHand << " > " << flush;
            break;
        case 's':
            action = pa_stand;
            break;
        case 't':
            destructiveBackspace(3);
            cout << endl;
            printStats();
            cout << playerHand << " > " << flush;
            break;
        };

        if ((action != pa_none) && (action != recommendation))
        {
            bell();
            bell();
            destructiveBackspace(3);
            printRecommendation(recommendation);
            cout << playerHand << " > " << flush;
            playerCorrect = false;
        }

        switch (action)
        {
        case pa_hit:
            hit(playerHand);
            break;
        case pa_stand:
            destructiveBackspace(3);
            stand = true;
            break;
        case pa_double:
            _playerCash -= playerBet;
            playerBet *= 2;
            hit(playerHand);
            destructiveBackspace(3);
            stand = true;
            break;
        case pa_split:
            destructiveBackspace(3);
            split = true;
            break;
        case pa_none:
            break;
        }
    }
#if UTL_HOST_OS != UTL_OS_MINGW
    hostOS->setCanonicalTTY(0, true);
#endif
    cout << "  ==>  ";
    if (split)
    {
        cout << "<<SPLIT>>";
    }
    else
    {
        cout << playerHand.value();
        if (playerHand.isBust())
        {
            cout << "  <<BUST>>";
        }
    }
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::hit(Hand& playerHand)
{
    const Card* card = _shoe.draw();
    playerHand += card;
    backspace(3);
    cout << ", " << card->faceString() << " > " << flush;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::dealer(Hand& dealerHand)
{
    cout << dealerHand << flush;
    hostOS->sleep(1);
    while (dealerHand.value() < 17)
    {
        const Card* card = _shoe.draw();
        dealerHand += card;
        cout << ", " << card->faceString() << flush;
        hostOS->sleep(1);
    }
    cout << "  ==>  " << dealerHand.value();
    if (dealerHand.isBust())
    {
        cout << "  <<BUST>>";
    }
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::assess(Hand& dealerHand, Hand& playerHand, double playerBet, bool playerCorrect)
{
    hand_result_t hres;

    // if player is bust, they lose regardless of dealer's hand
    if (playerHand.isBust())
    {
        hres = hr_lose;
    }
    // otherwise the hands are compared normally
    else
    {
        int handCmp = playerHand.compare(dealerHand);
        if (handCmp < 0)
            hres = hr_lose;
        else if (handCmp == 0)
            hres = hr_push;
        else
        {
            hres = hr_win;
            // if player wins and has blackjack payout is 3:2
            if (!playerHand.isSplit() && playerHand.isBlackjack())
            {
                hres = hr_win_blackjack;
            }
        }
    }

    _numHands++;
    if (playerCorrect)
    {
        _numHandsCorrect++;
        _totalBetCorrect += playerBet;
    }
    else
    {
        _totalBetIncorrect += playerBet;
    }

    // take action based on result of hand comparison
    const String& playerName = playerHand.getName();
    cout << "=== " << playerName << " ";
    switch (hres)
    {
    case hr_win:
        cout << "wins!" << endl;
        _playerCash += (2 * playerBet);
        if (playerCorrect)
        {
            _numHandsWonCorrect++;
            _totalWinningsCorrect += playerBet;
        }
        else
        {
            _numHandsWonIncorrect++;
            _totalWinningsIncorrect += playerBet;
        }
        break;
    case hr_win_blackjack:
        cout << "wins with Blackjack!" << endl;
        _playerCash += (playerBet * 2.5);
        if (playerCorrect)
        {
            _numHandsWonCorrect++;
            _totalWinningsCorrect += (playerBet * 1.5);
        }
        else
        {
            _numHandsWonIncorrect++;
            _totalWinningsIncorrect += (playerBet * 1.5);
        }
        break;
    case hr_lose:
        cout << "loses." << endl;
        if (playerCorrect)
        {
            _totalWinningsCorrect -= playerBet;
        }
        else
        {
            _totalWinningsIncorrect -= playerBet;
        }
        break;
    case hr_push:
        cout << "ties the house." << endl;
        _playerCash += playerBet;
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
BlackjackTrainer::getRecommendation(const Card* dealerCard,
                                    const Hand& playerHand,
                                    double playerBet) const
{
    uint_t dealerCardIdx = dealerCard->faceValueIdx();

    if (playerHand.isSplittable())
    {
        const Card* playerCard = (const Card*)playerHand.getCards().first();
        uint_t playerCardIdx = playerCard->faceValueIdx();
        if (splitTable[playerCardIdx][dealerCardIdx])
        {
            return pa_split;
        }
    }

    bool soft;
    uint_t total = playerHand.value(&soft);
    uint_t action;

    // soft total?
    if (soft)
    {
        if (total <= 12)
        {
            return pa_hit;
        }
        if (total >= 19)
        {
            return pa_stand;
        }
        uint_t totalIdx = total - 13;
        action = softTable[totalIdx][dealerCardIdx];
    }
    // hard total
    else
    {
        if (total <= 8)
        {
            return pa_hit;
        }
        if (total >= 17)
        {
            return pa_stand;
        }
        uint_t totalIdx = total - 9;
        action = hardTable[totalIdx][dealerCardIdx];
    }

    // determine recommendation
    bool canDouble = (playerHand.numCards() == 2) && (_playerCash >= playerBet);
    if (action == pa_double_or_hit)
    {
        if (canDouble)
            action = pa_double;
        else
            action = pa_hit;
    }
    else if (action == pa_double_or_stand)
    {
        if (canDouble)
            action = pa_double;
        else
            action = pa_stand;
    }

    return action;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::printRecommendation(uint_t recommendation) const
{
    cout << endl << "= Recommendation: " << String(actionStrings[recommendation]).toUpper() << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::printStats() const
{
    if (_numHands == 0)
    {
        cout << endl;
        cout << "<no statistics yet!>" << endl;
        cout << endl;
        return;
    }

    // calculate statistics

    // overall stats
    uint_t numHandsWon = _numHandsWonCorrect + _numHandsWonIncorrect;
    uint_t numHandsTied = _numHandsTiedCorrect + _numHandsTiedIncorrect;
    uint_t numHandsLost = _numHands - numHandsWon - numHandsTied;
    double percentHandsWon = (double)numHandsWon / _numHands;
    double percentHandsTied = (double)numHandsTied / _numHands;
    double percentHandsLost = (double)numHandsLost / _numHands;
    double totalBet = _totalBetCorrect + _totalBetIncorrect;
    double totalReturn = _totalWinningsCorrect + _totalWinningsIncorrect;
    double roi = totalReturn / totalBet;

    // stats for hands played correctly
    double percentHandsCorrect = (double)_numHandsCorrect / _numHands;
    uint_t numHandsLostCorrect = _numHandsCorrect - _numHandsWonCorrect - _numHandsTiedCorrect;
    double percentHandsWonCorrect = (double)_numHandsWonCorrect / _numHandsCorrect;
    double percentHandsTiedCorrect = (double)_numHandsTiedCorrect / _numHandsCorrect;
    double percentHandsLostCorrect = (double)numHandsLostCorrect / _numHandsCorrect;
    double roiCorrect = _totalWinningsCorrect / _totalBetCorrect;

    // stats for hands played incorrectly
    uint_t numHandsIncorrect = _numHands - _numHandsCorrect;
    double percentHandsIncorrect = (double)numHandsIncorrect / _numHands;
    uint_t numHandsLostIncorrect =
        numHandsIncorrect - _numHandsTiedIncorrect - _numHandsWonIncorrect;
    double percentHandsWonIncorrect = (double)_numHandsWonIncorrect / numHandsIncorrect;
    double percentHandsTiedIncorrect = (double)_numHandsTiedIncorrect / numHandsIncorrect;
    double percentHandsLostIncorrect = (double)numHandsLostIncorrect / numHandsIncorrect;
    double roiIncorrect = _totalWinningsIncorrect / _totalBetIncorrect;

    cout << endl;
    cout << "--- Overall -----------------------------------------" << endl;
    cout << "Hands played:      " << _numHands << endl;
    cout << "Hands won:         " << numHandsWon << " : (" << percent(percentHandsWon) << ")"
         << endl;
    cout << "Hands tied:        " << numHandsTied << " : (" << percent(percentHandsTied) << ")"
         << endl;
    cout << "Hands lost:        " << numHandsLost << " : (" << percent(percentHandsLost) << ")"
         << endl;
    cout << "Return:            " << money(totalReturn) << " / " << money(totalBet) << " : "
         << percent(roi) << endl;

    cout << "--- Correctly played hands --------------------------" << endl;
    cout << "Hands played:      " << _numHandsCorrect << " / " << _numHands << " : ("
         << percent(percentHandsCorrect) << ")" << endl;
    cout << "Hands won:         " << _numHandsWonCorrect << " : ("
         << percent(percentHandsWonCorrect) << ")" << endl;
    cout << "Hands tied:        " << _numHandsTiedCorrect << " : ("
         << percent(percentHandsTiedCorrect) << ")" << endl;
    cout << "Hands lost:        " << numHandsLostCorrect << " : ("
         << percent(percentHandsLostCorrect) << ")" << endl;
    cout << "Return:            " << money(_totalWinningsCorrect) << " / "
         << money(_totalBetCorrect) << " : " << percent(roiCorrect) << endl;

    cout << "--- Incorrectly played hands ------------------------" << endl;
    cout << "Hands played:      " << numHandsIncorrect << " / " << _numHands << " : ("
         << percent(percentHandsIncorrect) << ")" << endl;
    cout << "Hands won:         " << _numHandsWonIncorrect << " : ("
         << percent(percentHandsWonIncorrect) << ")" << endl;
    cout << "Hands tied:        " << _numHandsTiedIncorrect << " : ("
         << percent(percentHandsTiedIncorrect) << ")" << endl;
    cout << "Hands lost:        " << numHandsLostIncorrect << " : ("
         << percent(percentHandsLostIncorrect) << ")" << endl;
    cout << "Return:            " << money(_totalWinningsIncorrect) << " / "
         << money(_totalBetIncorrect) << " : " << percent(roiIncorrect) << endl;
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
BlackjackTrainer::money(double amount)
{
    String amountStr;
    if (amount < 0)
    {
        amountStr = "-";
        amount = fabs(amount);
    }
    amountStr += "$";
    amountStr += Float(amount).toString(2);
    return amountStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
BlackjackTrainer::percent(double pct)
{
    return Float(pct * 100).toString("precision:2") + '%';
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::bell()
{
    cout << (char)7 << flush;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::backspace(uint_t num)
{
    for (uint_t i = 0; i < num; i++)
    {
        cout << (char)8 << flush;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BlackjackTrainer::destructiveBackspace(uint_t num)
{
    for (uint_t i = 0; i < num; i++)
    {
        backspace();
        cout << ' ' << flush;
        backspace();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t BlackjackTrainer::splitTable[10][10] = {
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, // 2
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, // 3
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, // 4
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 5
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // 6
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 1, 0, 1, 1, 0, 0}, // 9
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // T
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // A
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define H pa_hit
#define S pa_stand
#define Dh pa_double_or_hit
#define Ds pa_double_or_stand

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t BlackjackTrainer::softTable[6][10] = {
    {H, H, H, Dh, Dh, H, H, H, H, H},   // 2
    {H, H, H, Dh, Dh, H, H, H, H, H},   // 3
    {H, H, Dh, Dh, Dh, H, H, H, H, H},  // 4
    {H, H, Dh, Dh, Dh, H, H, H, H, H},  // 5
    {H, Dh, Dh, Dh, Dh, H, H, H, H, H}, // 6
    {S, Ds, Ds, Ds, Ds, S, S, H, H, H}  // 7
};

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t BlackjackTrainer::hardTable[8][10] = {
    {H, Dh, Dh, Dh, Dh, H, H, H, H, H},      // 9
    {Dh, Dh, Dh, Dh, Dh, Dh, Dh, Dh, H, H},  // 10
    {Dh, Dh, Dh, Dh, Dh, Dh, Dh, Dh, Dh, H}, // 11
    {H, H, S, S, S, H, H, H, H, H},          // 12
    {S, S, S, S, S, H, H, H, H, H},          // 13
    {S, S, S, S, S, H, H, H, H, H},          // 14
    {S, S, S, S, S, H, H, H, H, H},          // 15
    {S, S, S, S, S, H, H, H, H, H}           // 16
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef H
#undef S
#undef Dh
#undef Ds

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* BlackjackTrainer::actionStrings[5] = {"hit", "stand", "double down", "split pair"};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(TArray, Card);
UTL_INSTANTIATE_TPL(Stack, Card);
