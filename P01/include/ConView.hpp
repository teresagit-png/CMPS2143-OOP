#pragma once
#include "VidPoker.hpp"
#include "GameView.hpp"

class ConsoleView : public GameView {
private:
    VideoPoker& game;

public:
    explicit ConsoleView(VideoPoker& game);

    void run() override;
};