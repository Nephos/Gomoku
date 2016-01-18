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
    void setCookie(std::string const &);
    void resetGame();
    void updateMap(std::istringstream &);

  protected:
    Player(std::string const &, std::string const &);
    ~Player() {};
    static Player *p;

    std::string _color;
    std::string _host;
    std::string _cookie;
    bool _win;
    bool _myTurn;
    bool _gameOver;
    int _whiteScore;
    int _blackScore;
    Network _network;
    std::map<std::pair<int, int>, char> _map;
};
