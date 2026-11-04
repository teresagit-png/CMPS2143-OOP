#include "tuiView.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

std::string GetRank(int value){
    const std::string ranks[] = {
        "A", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "J", "Q", "K"
    };
    return ranks[value % 13];
}

std::string GetSuit(int value) {
    const std::string suits[] = {
        "♠", "♥", "♦︎", "♣"
    };
    return suits[value / 13];
}

TuiView::TuiView(VideoPoker& game) : game(game) {}

Element DrawCard(const std::string& rank, const std::string& suit, 
                 bool held, bool selected) {

  auto suit_color =
      (suit == "♥" || suit == "♦") ? Color::Red : Color::White;

  auto body = vbox({hbox({text(rank), filler(), text(suit)}),
                    filler(),
                    text(suit) | bold | center,
                    filler(),
                    hbox({text(suit), filler(), text(rank)})}) |
              color(suit_color) | size(WIDTH, EQUAL, 9) |
              size(HEIGHT, EQUAL, 7);

  // Different border for cursor position.
  if (selected)
    body = body | borderDouble;
  else
    body = body | border;

  // Put HOLD underneath independently of selection.
  return vbox({body, held ? text("HOLD") | bold | center : text("    ")});
}

void TuiView::run(){
    auto screen = ScreenInteractive::TerminalOutput();

    int selected = 0;
    std::string message = "Place your bet to begin ";
    
    auto renderer = Renderer([&] {
        Elements cards;
        for(int i = 0; i < game.hand().size(); ++i){
            int value = game.hand().at(i).getValue();

            cards.push_back(
                DrawCard(
                    GetRank(value),
                    GetSuit(value),
                    game.isHeld(i),
                    i == selected
                )
            );
            if(i != game.hand().size() - 1)
                cards.push_back(text(""));
        }
        return vbox({

                text("VIDEO POKER") | bold | center, 
                separator(),
                text("Credits: " + std::to_string(game.credits())) | center,
                separator(),
                hbox(std::move(cards)) | center, 
                separator(),
                text(message) | center, 
                separator(),
                game.phase() == Phase::Betting ? text("1-5: Bet  Q: Quit")
                | center : text(" "),
                game.phase() == Phase::Holding ? hbox({
                    text("←     →"),
                    text(" Move     "),
                    text("SPACE"),
                    text(" Hold     "),
                    text("Q"),
                    text(" Quit")
                }) | dim | center : text(""),

                //Game Over message
                game.phase() == Phase::GameOver ? text("GAME OVER :(")
                | bold | center : text("")

            }) | border;
    });

    auto app = CatchEvent(renderer, [&](Event event) {
        //Quit.
        if (event == Event::Character('q') ||
                event == Event::Character('Q')) {

                screen.Exit();
                return true;
            }

        //Betting
        if(game.phase() == Phase::Betting){
            if(event == Event::Character('1') ||
            event == Event::Character('2') ||
            event == Event::Character('3') ||
            event == Event::Character('4') ||
            event == Event::Character('5')){
                int bet = event.character() [0] - '0';

                if(game.deal(bet)){
                    message = "Select cards to HOLD";
                    selected = 0;
                }
                else{
                    message = "Invalid bet";
                }
                return true;
            }
        }
        // ----------------------------
        // Move cursor
        // ----------------------------
        if(game.phase() == Phase::Holding){
            if (event == Event::ArrowLeft) {
                selected--;

                if (selected < 0)
                    selected = 4;

                return true;
            }

            if (event == Event::ArrowRight) {

            selected++;

            if (selected > 4)
                selected = 0;

            return true;
            }
        
            // ----------------------------
            // Toggle HOLD
            // ----------------------------

            if (event == Event::Character(' ')) {
                game.toggleHold(selected);

                message = game.isHeld(selected) ? "Card HELD" : "Card RELEASED";
                return true;
            }

            // ----------------------------
            // Draw replacement cards
            // ----------------------------
            if (event == Event::Return) {
                game.draw();
                message = VideoPoker::rankName(game.lastResult()) + 
                " - Payout: " + std::to_string(game.lastPayout());

            return true;
            }
        }
        return false;
    });
    screen.Loop(app);
}