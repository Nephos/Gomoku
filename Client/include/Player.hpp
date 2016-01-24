#pragma once

#include <string>
#include <utility>
#include <map>
#include <tuple>
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
  int getColor() const;
  void sendClick(std::pair<int, int>, std::string const &);

protected:
  Player(std::string const &, std::string const &);
  ~Player() {};
  static Player *p;

  std::string _color;
  int _colorValue;
  std::string _host;
  std::string _cookie;
  bool _win;
  bool _myTurn;
  bool _gameOver;
  int _whiteScore;
  int _blackScore;
  Network _network;
  char _map[19][19];
  std::tuple<int, int> _lastMove;
};
