#pragma once

#include <string>
#include <utility>
#include <map>
#include "Display.hpp"
#include "Network.hpp"
#include "Player.hpp"

class Human : public Player
{
  public:
    void play();

    static Human *getInstance(bool rainbows = true, bool rotate = false, std::string const &host = "127.0.0.1", std::string const &port = "8080");

    bool parseAnswer(std::string const &);
    void resetGame();

  private:
    Human(std::string const &, std::string const &, bool, bool);
    ~Human() {};
    static Human *p;

    GomokuDisplay _display;
};
