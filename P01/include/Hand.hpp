#pragma once

#include <vector>
#include "Card.hpp"
#include "Deck.hpp"

class Hand {
    std::vector<Card> cards;
    
public:
    // Adds a card to the hand
    void addCard(const Card& c);
    // Removes all cards from hand
    void clear();
    // Returns the number of cards in hand
    int size() const;
    // Return a card at index i
    const Card& at(int i) const;
    // Replaces card at index i with a new card
    void replace(int i, const Card& c);
};
