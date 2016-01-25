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

int Computer::computeHeuristic() {
  static std::pair<int, int> dirs[] = {
    {-1, -1}, // North West
    {0, -1}, // North... I think ?
    {1, -1}, // North East
    {1, 0}, // East I guess
  };

  std::cout << std::endl;
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      std::cout << _map[i][j] << " ";
    }
    std::cout << std::endl;
  }

  bool used[4][19][19] = {false}; // To remember whether the tile was already checked in this direction
  int res = 0;
  for (int i = -2; i < 3; i++) {
    for (int j = -2; j < 3; j++) {
      int newx = _tree_x + j;
      int newy = _tree_y + i;
      if (newx < 0 || newx > 18
        || newy < 0 || newy > 18)
        continue;
      if (_map[newy][newx] == 'x')
        continue;
      for (int dir = 0; dir < 4; dir++) {
        if (used[dir][newy][newx] == true)
          continue;
        used[dir][newy][newx] = true;
        // Dir in one way and then in the other
        // Looking for lines here...
        int line = 1;
        int free = 0;

        for (int n = 1; n < 5; n++) {
          int tmpx = newx + dirs[dir].first * n;
          int tmpy = newy + dirs[dir].second * n;
          if (tmpx < 0 || tmpx > 18
            || tmpy < 0 || tmpy > 18)
            break; // Out of bounds

          if (used[dir][tmpy][tmpx] == true)
            continue;
          if (_map[tmpy][tmpx] != _map[newy][newx]) {
            if (_map[tmpy][tmpx] == 'x')
              free++;
            break; // End of pattern
          }
          used[dir][tmpy][tmpx] = true;
          line++;
        }
        // In the other way
        for (int n = 1; n < 5; n++) {
          int tmpx = newx + -dirs[dir].first * n;
          int tmpy = newy + -dirs[dir].second * n;
          if (tmpx < 0 || tmpx > 18
            || tmpy < 0 || tmpy > 18)
            break; // Out of bounds

          if (used[dir][tmpy][tmpx] == true)
              continue;
          if (_map[tmpy][tmpx] != _map[newy][newx]) {
            if (_map[tmpy][tmpx] == 'x')
              free++;
            break; // End of pattern
          }
          used[dir][tmpy][tmpx] = true;
          line++;
        }
        if (line < 2)
          continue;
        if (_map[newy][newx] == _colorValue) {
          if (line == 5)
            res += 100;
          if (free == 0) // Not interesting...
            continue;
          if (line == 3)
            res += 1 * (10 * free);
          else if (line == 4)
            res += 2 * (10 * free);
        }
        else {
          if (line == 5)
            res -= 100;
          if (free == 0)
            continue;
          if (line == 3)
            res -= 1 * (10 * free);
          else if (line == 4)
            res -= 2 * (10 * free);
        }
        std::cout << "Found a line of " << line << " which is free " << free << " times. (";
        if (_map[newy][newx] == _colorValue)
          std::cout << "mine)";
        else
          std::cout << "not mine)";
        std::cout << " in " << newy << " " << newx << std::endl;
      }
    }
  }
  return res;
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
  while (!_gameOver) { // Game loop
    ans = _network.getAnswer();
    parseAnswer(ans);
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
	computesMinMax(TREE_DEEPTH, _colorValue);
	// No response
	if (_best_x == -1) {
	  setRandomBestPosition();
	}
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
