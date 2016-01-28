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

#include <ctime>
void Computer::play() {
  Player::play();
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  initializeMinMax();
  printf("|  MinMax  | Arbitre  |  Total   | duration (s)\n");
  printf("|----------|----------|----------|\n");
  while (!_gameOver) { // Game loop
    ans = _network.getAnswer();
    parseAnswer(ans);
    clock_t tStart1 = clock();
    if (!_myTurn) {
      std::string req = "GET /game.txt" + header + _cookie + "\r\n\r\n";
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
    }
    else {
      if (_moveFailed == true) {
	setRandomBestPosition();
	_moveFailed = false;
      }
      else if (std::get<0>(_lastMove) == -1) {
	_best_x = 9;
	_best_y = 9;
      }
      else {
	_best_x = -1;
	// The opponent moved
	setMoveToXY(_colorValue ^ 1, std::get<0>(_lastMove), std::get<1>(_lastMove));
	// We move
	computesMinMax(TREE_DEPTH, _colorValue, ABMIN, ABMAX);
	// No response
	if (_best_x == -1) {
	  setRandomBestPosition();
	}
      }
      clock_t tEnd1 = clock();
      setMoveToXY(_colorValue, _best_x, _best_y);
      std::stringstream ss;
      ss << "POST /game/play/" << _best_x << "/" << _best_y << header << _cookie << "\r\n\r\n";
      std::string req = ss.str();
      clock_t tStart2 = clock();
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
      clock_t tEnd2 = clock();
      printf("| %.6f | %.6f | %.6f | \n", (double)(tEnd1 - tStart1)/CLOCKS_PER_SEC, (double)(tEnd2 - tStart2)/CLOCKS_PER_SEC, (double)(tEnd2 - tStart1)/CLOCKS_PER_SEC);
      req = "GET /game/map.txt" + header + _cookie + "\r\n\r\n";
      ans = _network.sendSyncQuery(req);
      parseAnswer(ans);
      _myTurn = false;
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
  static int xplayed = -1;
  static int yplayed = -1;
  if (xplayed != _best_x1 || yplayed != _best_y1) {
    _best_x = _best_x1;
    _best_y = _best_y1;
    xplayed = _best_x1;
    yplayed = _best_y1;
    return;
  }
  for (int y = 0; y < 19; y++) {
    for (int x = 0; x < 19; x++) {
      if (_map[y][x] == 'x' && _usables[y][x] > 0) {
        _best_x = x;
        _best_y = y;
        return;
      }
    }
  }
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
