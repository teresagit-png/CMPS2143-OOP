#include "VidPoker.hpp"

VideoPoker::VideoPoker(int startingCredits)
    :   creditBalance(startingCredits < 0 ? 0 : startingCredits),
        currentBet(0),
        currentPhase(startingCredits <= 0 ? Phase::GameOver : Phase::Betting),
        currentHand(),
        held(),
        result(HandRank::Nothing),
        payout(0),
        currentDeck(std::make_unique<Deck>())
{
    currentDeck->shuffle();
}

bool VideoPoker::deal(int bet){
    if(!currentDeck) {
        currentDeck = nextTestDeck ? std::move(nextTestDeck)
                                   : std::make_unique<Deck>();
        if(!nextTestDeck && currentDeck)
            currentDeck->shuffle();
    }

    //Only allow betting during the betting phase
    if(currentPhase != Phase::Betting)
        return false;

    //Bets must be between 1 and 5 credits
    if(bet < 1 || bet > 5 || bet > creditBalance)
        return false;

    //Pay the bet immediately
    creditBalance -= bet;
    currentBet += bet;

    currentHand.clear();
    held.assign(5, false);

    result = HandRank::Nothing;
    payout = 0;

    //Deal five cards
    for(int i = 0; i < 5; ++i)
        currentHand.addCard(currentDeck->deal());

    currentPhase = Phase::Holding;
    return true;
}

bool VideoPoker::toggleHold(int index){
    if(currentPhase != Phase::Holding) 
        return false;
    if(index < 0 || index >= currentHand.size())
        return false;
    held[index] = !held[index];
    return true;
}
bool VideoPoker::draw(){
    if(currentPhase != Phase::Holding){
        return false;
    }
    //Replace every card that isn't being held
    for(int i = 0; i < currentHand.size(); ++i){
        if(!held[i]){
            currentHand.replace(i, currentDeck->deal());
        }
    }
    //Evaluate the final five card hand
    result = evaluate(currentHand);
    //Calulate the total payout for this hand
    payout = multiplier(result, currentBet);
    creditBalance += payout;

    if(creditBalance == 0)
        currentPhase = Phase::GameOver;
    else
        currentPhase = Phase::Betting;

    currentBet = 0;
    currentDeck.reset();
    held.clear();
    return true;
}

// Getters
Phase VideoPoker::phase() const {
    return currentPhase;
}
int VideoPoker::credits() const {
    return creditBalance;
}
int VideoPoker::currentBetAmount() const{
    return currentBet;
}
const Hand& VideoPoker::hand() const{
    return currentHand;
}
bool VideoPoker::isHeld(int index) const{
    if(index < 0 || index >= static_cast<int>(held.size())){
        return false;
    }
    return held[index];
}
HandRank VideoPoker::lastResult() const{
    return result;
}
int VideoPoker::lastPayout() const{
    return payout;
}
void VideoPoker::setNextDeck(Deck deck){
    nextTestDeck = std::make_unique<Deck>(std::move(deck));
}

// C++
HandRank VideoPoker::evaluate(const Hand& h) {
    if (h.size() != 5)
        return HandRank::Nothing;

    std::array<int, 13> counts{};
    bool flush = true;
    const int firstSuit = h.at(0).getValue() / 13;

    for (int i = 0; i < 5; ++i) {
        const Card& card = h.at(i);
        const int rank = card.getRank();

        ++counts[rank];

        if (card.getSuit() != firstSuit)
            flush = false;
    }

    int pairs = 0;
    bool threeOfAKind = false;
    bool fourOfAKind = false;

    for (int count : counts) {
        if (count == 2)
            ++pairs;
        else if (count == 3)
            threeOfAKind = true;
        else if (count == 4)
            fourOfAKind = true;
    }

    const bool aceLowStraight =
        counts[0] == 1 && counts[1] == 1 && counts[2] == 1 &&
        counts[3] == 1 && counts[4] == 1;

    const bool aceHighStraight =
        counts[0] == 1 && counts[9] == 1 && counts[10] == 1 &&
        counts[11] == 1 && counts[12] == 1;

    bool ordinaryStraight = false;
    for (int start = 1; start <= 8; ++start) {
        bool valid = true;

        for (int offset = 0; offset < 5; ++offset) {
            if (counts[start + offset] != 1) {
                valid = false;
                break;
            }
        }

        if (valid) {
            ordinaryStraight = true;
            break;
        }
    }

    const bool straight = aceLowStraight || aceHighStraight || ordinaryStraight;
    const bool royal = aceHighStraight;

    if (flush && royal)
        return HandRank::RoyalFlush;
    if (flush && straight)
        return HandRank::StraightFlush;
    if (fourOfAKind)
        return HandRank::FourOfAKind;
    if (threeOfAKind && pairs == 1)
        return HandRank::FullHouse;
    if (flush)
        return HandRank::Flush;
    if (straight)
        return HandRank::Straight;
    if (threeOfAKind)
        return HandRank::ThreeOfAKind;
    if (pairs == 2)
        return HandRank::TwoPair;

    if (pairs == 1 &&
        (counts[10] == 2 || counts[11] == 2 || counts[12] == 2)) {
        return HandRank::JacksorBetter;
    }

    return HandRank::Nothing;
}

int VideoPoker::multiplier(HandRank r, int bet){
    if(bet < 1 || bet > 5)
        return 0;
    switch(r){
        case HandRank::RoyalFlush:
            return(bet == 5 ? 4000 : 250 * bet);
        case HandRank::StraightFlush:
            return 50 * bet;
        case HandRank::FourOfAKind:
            return 25 * bet;
        case HandRank::FullHouse:
            return 9 * bet;
        case HandRank::Flush:
            return 6 * bet;
        case HandRank::Straight:
            return 4 * bet;
        case HandRank::ThreeOfAKind:
            return 3 * bet;
        case HandRank::TwoPair:
            return 2 * bet;
        case HandRank::JacksorBetter:
            return 1 * bet;
        case HandRank::Nothing:
        default:
            return 0;
    }
}
std::string VideoPoker::rankName(HandRank r){
    switch(r){
        case HandRank::RoyalFlush:
            return "Royal Flush";
        case HandRank::StraightFlush:
            return "Straight Flush";
        case HandRank::FourOfAKind:
            return "Four of a Kind";
        case HandRank::FullHouse:
            return "Full House";
        case HandRank::Flush:
            return "Flush";
        case HandRank::Straight:
            return "Straight";
        case HandRank::ThreeOfAKind:
            return "Three of a Kind";
        case HandRank::TwoPair:
            return "Two Pair";
        case HandRank::JacksorBetter:
            return "Jacks or Better";
        case HandRank::Nothing:
            return "Nothing";
        default:
            return "Unknown";
    }
}
