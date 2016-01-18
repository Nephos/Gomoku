#pragma once

#include <string>
#include <utility>
#include <map>
#include "Player.hpp"

class Computer : public Player
{
  public:
    void play();

    static Computer *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080");

    bool parseAnswer(std::string const &);
    void resetGame();
    int computesMinMax(int deepth_max, int current_color);

  private:
    Computer(std::string const &, std::string const &);
    ~Computer() {};
    static Computer *p;
};
