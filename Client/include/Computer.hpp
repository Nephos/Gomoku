#pragma once

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <stack>
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

    std::vector<int> _weights;
    // true / false if tile is usable ... TODO: more data
    std::vector<bool> _cache;
    // <color, position <<x, y>>
    std::stack<std::pair<int, std::pair<int, int> > > _stack;
};
