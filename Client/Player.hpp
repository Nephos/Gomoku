#pragma once

#include <string>
#include <utility>
#include "Network.hpp"

class Player
{
  public:
    void play();

    static Player *getInstance(std::string const &colour = "", std::string const &host = "127.0.0.1", std::string const &port = "8080");

  private:
    Player(std::string const &, std::string const &, std::string const &);
    ~Player() {};
    static Player *p;

    std::string _colour;
    std::string _host;
    Network _network;
};