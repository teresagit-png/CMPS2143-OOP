#include "VidPoker.hpp"
#include "GameView.hpp"
#include "ConView.hpp"
#include "tuiView.hpp"
#include "Tests.cpp"

using namespace std;

int main(int argc, char* argv[]) {

    // Run automated tests.
    if (argc > 1 && string(argv[1]) == "--test") {
        return runTests();
    }

    VideoPoker game(100);

    unique_ptr<GameView> view;

    // Console View.
    if (argc > 1 && string(argv[1]) == "--text") {
        view = make_unique<ConsoleView>(game);
    }
    // TUI View (default).
    else {
        view = make_unique<TuiView>(game);
    }

    view->run();

    return 0;
}