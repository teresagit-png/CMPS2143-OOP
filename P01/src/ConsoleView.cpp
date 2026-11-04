#include "ConView.hpp"
#include <iostream>

ConsoleView::ConsoleView(VideoPoker& game) : game(game) {}

void ConsoleView::run(){
    bool playing = true;

    while(playing){
        std::cout << "Credits: " << game.credits() << std::endl;

        //Betting Phase
        if(game.phase() == Phase::Betting){
            std::cout << "Bet (1-5; 0 to quit): ";
            
            int bet;
            std::cin >> bet;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input.\n";
                continue;
            }

            if (bet == 0) {
                playing = false;
            }
            else if (!game.deal(bet)) {
                std::cout << "Invalid bet. Try again.\n";
            }
        }

        //Holding phase
        else if(game.phase() == Phase::Holding){
            for(int i = 0; i < game.hand().size(); i++){
                std::cout << " [" << i + 1 << "]" 
                << game.hand().at(i).toString();

                if(game.isHeld(i)){
                    std::cout << "*";
                }
            }

            std::cout << "\nEnter card number (1-5) to hold/unhold. ";
            std::cout << "Enter 0 to draw:  ";

            int choice;
            std::cin >> choice;

                if(choice == 0){
                    game.draw();
                    for(int i = 0; i < game.hand().size(); i++){
                        const Card& card = game.hand().at(i);
                        std::cout << " [" << i + 1 << "]" 
                        << card.toString();

                        if(game.isHeld(i)){
                            std::cout << "*";
                        }
                    }
                    
                    std::cout << "\nHand result: " 
                        << VideoPoker::rankName(game.lastResult()) << std::endl;
                    std::cout << "Payout: " << game.lastPayout() << std::endl;
                }
                else if(!game.toggleHold(choice - 1))
                    std::cout << "Invalid card number.\n";
        }
        //Game Over Phase
        else if(game.phase() == Phase::GameOver){
            std::cout << "========GAME OVER========\n";
            std::cout << "Final credits: " << game.credits() << std::endl;
            playing = false;
        }
    }
    std::cout << "\nThanks for playing!!\n";
}