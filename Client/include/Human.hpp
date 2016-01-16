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
    void connect();

    static Human *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080",
                              std::string const &cookie = "", std::string const &color = "");

    bool parseAnswer(std::string const &);
    void resetGame();
    std::pair<int, int> updateMap(std::istringstream &);
    void sendClick(std::pair<int, int>, std::string const &);

  private:
    Human(std::string const &, std::string const &, std::string const &, std::string const &);
    ~Human() {};
    static Human *p;

    GomokuDisplay _display;
};
