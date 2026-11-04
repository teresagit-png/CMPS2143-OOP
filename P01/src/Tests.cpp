
#include "VidPoker.hpp"
#include "Hand.hpp"
#include "Card.hpp"
#include "Deck.hpp"

#include <iostream>
#include <vector>
#include <initializer_list>

// Creates a Hand from a list of card values.
Hand makeHand(std::initializer_list<int> values) {
    Hand hand;

    for (int value : values) {
        hand.addCard(Card(value));
    }

    return hand;
}

// Prints one PASS or FAIL line for each test.
void check(const std::string& testName, bool passed, int& failures) {
    if (passed) {
        std::cout << "PASS: " << testName << '\n';
    } else {
        std::cout << "FAIL: " << testName << '\n';
        failures++;
    }
}

int runTests() {
    int failures = 0;

    std::cout << "\n---------Video Poker Tests---------\n\n";
    // --------------------------------------------------
    // Test every HandRank
    // --------------------------------------------------

    check("Nothing",
        VideoPoker::evaluate(makeHand({0, 5, 8, 17, 25}))
            == HandRank::Nothing, failures);

    check("JacksOrBetter",
        VideoPoker::evaluate(makeHand({10, 23, 5, 8, 12}))
            == HandRank::JacksorBetter, failures);

    check("TwoPair",
        VideoPoker::evaluate(makeHand({10, 23, 5, 18, 12}))
            == HandRank::TwoPair, failures);

    check("ThreeOfAKind",
        VideoPoker::evaluate(makeHand({10, 23, 36, 5, 8}))
            == HandRank::ThreeOfAKind, failures);

    check("Straight",
        VideoPoker::evaluate(makeHand({0, 14, 2, 16, 4}))
            == HandRank::Straight, failures);

    check("Flush",
        VideoPoker::evaluate(makeHand({0, 2, 5, 7, 11}))
            == HandRank::Flush, failures);

    check("FullHouse",
        VideoPoker::evaluate(makeHand({10, 23, 36, 5, 18}))
            == HandRank::FullHouse, failures);

    check("FourOfAKind",
        VideoPoker::evaluate(makeHand({10, 23, 36, 49, 5}))
            == HandRank::FourOfAKind, failures);

    check("StraightFlush",
        VideoPoker::evaluate(makeHand({4, 5, 6, 7, 8}))
            == HandRank::StraightFlush, failures);

    check("RoyalFlush",
        VideoPoker::evaluate(makeHand({0, 9, 10, 11, 12}))
            == HandRank::RoyalFlush, failures);


    // --------------------------------------------------
    // Test pairs
    // --------------------------------------------------

    /****/check("Pair of 10s is Nothing",
        VideoPoker::evaluate(makeHand({9, 22, 0, 4, 7}))
            == HandRank::Nothing, failures);

    check("Pair of Jacks is JacksOrBetter",
        VideoPoker::evaluate(makeHand({10, 23, 0, 4, 7}))
            == HandRank::JacksorBetter, failures);


    // --------------------------------------------------
    // Test straights
    // --------------------------------------------------

    check("A-2-3-4-5 is a Straight",
        VideoPoker::evaluate(makeHand({0, 14, 2, 16, 4}))
            == HandRank::Straight, failures);

    /****/check("10-J-Q-K-A mixed suits is a Straight",
        VideoPoker::evaluate(makeHand({9, 23, 37, 51, 0}))
            == HandRank::Straight, failures);

    /****/check("Q-K-A-2-3 is NOT a Straight",
        VideoPoker::evaluate(makeHand({11, 25, 26, 40, 2}))
            == HandRank::Nothing, failures);


    // --------------------------------------------------
    // Test Royal Flush payouts
    // --------------------------------------------------

    check("Royal Flush pays 250 at bet 1",
        VideoPoker::multiplier(HandRank::RoyalFlush, 1) == 250,
        failures);

    check("Royal Flush pays 500 at bet 2",
        VideoPoker::multiplier(HandRank::RoyalFlush, 2) == 500,
        failures);

    check("Royal Flush pays 750 at bet 3",
        VideoPoker::multiplier(HandRank::RoyalFlush, 3) == 750,
        failures);

    check("Royal Flush pays 1000 at bet 4",
        VideoPoker::multiplier(HandRank::RoyalFlush, 4) == 1000,
        failures);

    check("Royal Flush pays 4000 at bet 5",
        VideoPoker::multiplier(HandRank::RoyalFlush, 5) == 4000,
        failures);


    // --------------------------------------------------
    // Test held cards and replacement cards using a
    // stacked deck.
    // --------------------------------------------------

    VideoPoker game(100);

    // First five cards are dealt into the hand.
    // The remaining four cards replace unheld cards.
    Deck stackedDeck({
        Card(0),
        Card(14),
        Card(28),
        Card(42),
        Card(4),

        Card(1),
        Card(15),
        Card(29),
        Card(43)
    });

    game.setNextDeck(std::move(stackedDeck));

    bool dealt = game.deal(1);

    check("Deal using stacked deck",
        dealt && game.hand().size() == 5, failures);

    // Save the original hand.
    std::vector<int> originalCards;

    for (int i = 0; i < game.hand().size(); i++) {
        originalCards.push_back(game.hand().at(i).getValue());
    }

    // Hold only the first card.
    bool held = game.toggleHold(0);

    check("Hold first card",
        held && game.isHeld(0), failures);

    // Draw replaces the other four cards.
    bool drawn = game.draw();

    check("Draw succeeds", drawn, failures);

    // The held card should remain unchanged.
    check("Held card survives draw",
        drawn && game.hand().at(0).getValue() == originalCards[0],
        failures);

    // Every unheld card should have been replaced.
    bool replaced = drawn;

    for (int i = 1; i < 5; i++) {
        if (game.hand().at(i).getValue() == originalCards[i]) {
            replaced = false;
        }
    }

    check("Unheld cards are replaced", replaced, failures);


    // --------------------------------------------------
    // Final result
    // --------------------------------------------------

    std::cout << "\n------------------------------------\n";

    if (failures == 0) {
        std::cout << "All tests passed!\n";
        return 0;
    }

    std::cout << failures << " test(s) failed.\n";
    return 1;
}
