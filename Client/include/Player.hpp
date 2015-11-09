#pragma once

#include <string>
#include <utility>
#include <map>
#include "Display.hpp"
#include "Network.hpp"

class Player
{
  public:
    void play();
    void connect();

    static Player *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080");

    void initMap();
    bool parseAnswer(std::string const &);
    void setCookie(std::string const &);
    void resetGame();
    void sendClick(std::pair<int, int>, std::string const &);

  private:
    Player(std::string const &, std::string const &);
    ~Player() {};
    static Player *p;

    std::string _color;
    std::string _host;
    std::string _cookie;
    bool _myTurn;
    bool _gameOver;
    Network _network;
    GomokuDisplay _display;
    std::map<std::pair<int, int>, char> _map;
};