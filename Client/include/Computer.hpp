#pragma once

#include <string>
#include <utility>
#include <map>
#include "Display.hpp"
#include "Network.hpp"

class Computer
{
  public:
    void play();
    void connect();

    static Computer *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080",
                              std::string const &cookie = "", std::string const &color = "");

    void initMap();
    bool parseAnswer(std::string const &);
    void setCookie(std::string const &);
    void resetGame();
    void updateMap(std::istringstream &);
    void sendClick(std::pair<int, int>, std::string const &);

  private:
    Computer(std::string const &, std::string const &, std::string const &, std::string const &);
    ~Computer() {};
    static Computer *p;

    std::string _color;
    std::string _host;
    std::string _cookie;
    bool _myTurn;
    bool _gameOver;
    Network _network;
    GomokuDisplay _display;
    std::map<std::pair<int, int>, char> _map;
};
