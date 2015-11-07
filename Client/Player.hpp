#pragma once

#include <string>
#include <utility>
#include "Network.hpp"

class Player
{
  public:
    void play();
    void connect();

    static Player *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080");

    bool parseAnswer(std::string const &);

  private:
    Player(std::string const &, std::string const &);
    ~Player() {};
    static Player *p;

    std::string _color;
    std::string _host;
    std::string _cookie;
    bool _myTurn;
    Network _network;
};