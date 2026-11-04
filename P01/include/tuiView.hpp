#pragma once

#include "GameView.hpp"
#include "VidPoker.hpp"

// A VIEW: shows the same game with FTXUI.
class TuiView : public GameView {
  VideoPoker &game;

public:
  explicit TuiView(VideoPoker &game);
  void run() override;
};
