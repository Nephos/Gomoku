#pragma once

#include <string>
#include <utility>
#include <map>
#include "Network.hpp"

class Player
{
  public:
    void play();
    void connect();

    void initMap();
    bool parseAnswer(std::string const &);
    std::string setCookie(std::string const &);
    void resetGame();
    std::pair<int, int> updateMap(std::istringstream &);

  protected:
    Player(std::string const &, std::string const &, std::string const &, std::string const &);
    ~Player() {};
    static Player *p;

    std::string _color;
    std::string _host;
    std::string _cookie;
    bool _win;
    bool _myTurn;
    bool _gameOver;
    Network _network;
    std::map<std::pair<int, int>, char> _map;
};
