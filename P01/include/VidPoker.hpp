#pragma once

#include "Hand.hpp"

enum class Phase { Betting, Holding, GameOver};
enum class HandRank { Nothing, JacksorBetter, TwoPair, ThreeOfAKind, 
                      Straight, Flush, FullHouse, FourOfAKind, 
                      StraightFlush, RoyalFlush};

class VideoPoker{
    int creditBalance;
    int currentBet;
    Phase currentPhase;
    Hand currentHand;
    std::vector<bool> held;
    HandRank result;
    int payout;
    std::unique_ptr<Deck> currentDeck;
    std::unique_ptr<Deck> nextTestDeck;
public:
    explicit VideoPoker(int startingCredits = 100);

   //game actions
   bool deal(int bet);
   bool toggleHold(int index);
   bool draw();
   
   //getters
   Phase phase() const;
   int credits() const;
   int currentBetAmount() const;
   const Hand& hand() const;
   bool isHeld(int index) const;
   HandRank lastResult() const;
   int lastPayout() const;

   //evaluate final hand and pay
   static HandRank evaluate(const Hand& h);
   static int multiplier(HandRank r, int bet);
   static std::string rankName(HandRank r);

   //Gives tests a predetermined deck
   void setNextDeck(Deck deck);
};