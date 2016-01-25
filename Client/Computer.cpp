#include <iostream>
#include <sstream>
#include "Computer.hpp"

Computer::Computer(std::string const &host, std::string const &port)
  : Player(host, port) {}

Computer *Computer::p = NULL;
Computer *Computer::getInstance(std::string const &host, std::string const &port) {
  if (p == NULL)
    p = new Computer(host, port);
  return p;
}

void Computer::resetGame() {
  // Reset les poids / l'IA ici
}

#define NEXT_ROUND_PREPARATION			\
  for (int y = 0; y < 19; y++) {		\
    for (int x = 0; x < 19; x++) {		\
      if (_usables[y][x] > 0) {			\
	_usables[y][x]--;			\
      }						\
    }						\
  }

void Computer::play() {
  Player::play();
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  initializeMinMax();
  while (true/* Something */) { // Game loop
    ans = _network.getAnswer();
    parseAnswer(ans);
    if (!_myTurn && !_gameOver) {
      std::string req = "GET /game.txt" + header + _cookie + "\r\n\r\n";
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
    }
    else if (!_gameOver) {
      if (std::get<0>(_lastMove) != -1) {
	setMoveToXY(_colorValue ^ 1, std::get<0>(_lastMove), std::get<1>(_lastMove));
	update_tile(_map, std::get<0>(_lastMove) + 19 * std::get<1>(_lastMove));
	_best_x = -1;
	computesMinMax(TREE_DEEPTH, _colorValue);
	if (_best_x == -1) {
	  setRandomBestPosition();
	}
      }
      // The opponent moved
      else {
	// Computes our move
	_best_x = 9;
	_best_y = 9;
      }
      setMoveToXY(_colorValue, _best_x, _best_y);
      std::stringstream ss;
      ss << "POST /game/play/" << _best_x << "/" << _best_y << header << _cookie << "\r\n\r\n";
      std::string req = ss.str();
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
      req = "GET /game/map.txt" + header + _cookie + "\r\n\r\n";
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
      _myTurn = false;
      // sendClick(p, header);
      NEXT_ROUND_PREPARATION;
#ifdef DEBUG
      for (int y = 0; y < 19; y++) {
	for (int x = 0; x < 19; x++) {
	  if (_usables[y][x])
	    std::cout << "Usable (" << x << ":" << y << ") = " << _usables[y][x] << std::endl;
	}
      }
#endif // DEBUG
    }
  }
}

void Computer::setRandomBestPosition() {
  for (int y = 0; y < 19; y++) {
    for (int x = 0; x < 19; x++) {
      if (_map[y][x] == 'x') {
	_best_x = x;
	_best_y = y;
	return;
      }
    }
  }
}

void Computer::setMoveToXY(int color, int x, int y) {
  int c = pushColorAt(color, x, y);
  while (c--)
    _stack.pop();
}

/* If returns false, then ask for another user input */
/* Else, wait for the other client */
bool Computer::parseAnswer(const std::string &str) {
  if (!Player::parseAnswer(str))
    return false;
  /* Different possible answers impacting the game
     if (_gameOver && _win){}
     else if (_gameOver && !_win){}
     else if (_myTurn){}
     else if (!_myTurn){}
  */
  return true;
}
