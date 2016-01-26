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
    // {1, 1}, // South East
    // {0, 1}, // South
    // {-1, 1}, // South West
    // {-1, 0} // West
  };
  bool used[4][19][19] = {false}; // To remember whether the tile was already checked in this direction
  int res = 0;
  for (int i = -2; i < 3; i++) {
    for (int j = -2; j < 3; j++) {
      int newx = _tree_x + j;
      int newy = _tree_y + i;
      if (newx < 0 || newx > 18
        || newy < 0 || newy > 18)
        continue;
      bool empty = false;
      if (_map[newy][newx] == 'x')
        empty = true;
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

          if (empty && n == 1)
            _map[newy][newx] = _map[tmpy][tmpx];
          if (_map[newy][newx] == 'x')
            break;
          if (used[dir][tmpy][tmpx] && !empty)
            break;
          if (_map[tmpy][tmpx] != _map[newy][newx]) {
            if (_map[tmpy][tmpx] == 'x')
              free++;
            break; // End of pattern
          }
          used[dir][tmpy][tmpx] = true;
          line++;
        }
        // In the other way
        // Except if we were on an empty tile...
        if (empty && line == 0)
          continue;
        int tmpline = 0;
        for (int n = 1; n < 5; n++) {
          int tmpx = newx + -dirs[dir].first * n;
          int tmpy = newy + -dirs[dir].second * n;
          if (tmpx < 0 || tmpx > 18
            || tmpy < 0 || tmpy > 18)
            break; // Out of bounds

          if (empty && n == 1) {
            if (_map[newy][newx] != _map[tmpy][tmpx]) {
              line = 0;
              free = 0;
              _map[newy][newx] = 'x';
              break;
            }
          }

          if (used[dir][tmpy][tmpx] && !empty)
            break;
          if (_map[tmpy][tmpx] != _map[newy][newx]) {
            if (_map[tmpy][tmpx] == 'x')
              free++;
            break; // End of pattern
          }
          used[dir][tmpy][tmpx] = true;
          line++;
          tmpline++;
        }
        if (empty && tmpline == 0) {
          // Undo everything ~
          for (int n = 1; n < 5; n++) {
            int tmpx = newx + dirs[dir].first * n;
            int tmpy = newy + dirs[dir].second * n;
            if (tmpx < 0 || tmpx > 18
              || tmpy < 0 || tmpy > 18)
              break; // Out of bounds

            if (empty && n == 1) {
              _map[newy][newx] = _map[newy + dirs[dir].second][newx + dirs[dir].first];
            }
            if (_map[newy][newx] == 'x')
              break;
            if (_map[tmpy][tmpx] != _map[newy][newx]) {
              break; // End of pattern
            }
            used[dir][tmpy][tmpx] = false;
          }
          _map[newy][newx] = 'x';
          continue;
        }

        /* Calculating weights */

        if (line < 2) {
          if (empty)
            _map[newy][newx] = 'x';
          continue;
        }
        if (_map[newy][newx] == _colorValue + '0') {
          int r = 0;
          if (line == 5)
            r = 1000;
          if (free == 0) { // Not interesting...
            if (empty)
              _map[newy][newx] = 'x';
            continue;
          }
          if (line == 2)
            r = 5 * free;
          else if (line == 3)
            r = 20 * free;
          else if (line == 4)
            r = 50 * free;
          if (empty)
            r = r / 1.5;
          res += r;
        }
        else {
          int r = 0;
          if (line == 5)
            r = 1000;
          if (free == 0) {
            if (empty)
              _map[newy][newx] = 'x';
            continue;
          }
          if (line == 2)
            r = 5 * free;
          else if (line == 3)
            r = 20 * free;
          else if (line == 4)
            r = 50 * free;
          if (empty)
            r = r / 1.5;
          res -= r;
        }
        // if (empty) {
        //   std::cout << "Found a " << (empty ? "potential " : "") << "line of " << line << " which is free " << free << " times. (";
        //   if (_map[newy][newx] == _colorValue)
        //     std::cout << "mine)";
        //   else
        //     std::cout << "not mine)";
        //   std::cout << " in " << newy << " " << newx << std::endl;
        // }
        if (empty)
          _map[newy][newx] = 'x';
      }
    }
  }
  // Finally, we take into account the tokens taken during the simulation.
  if (_colorValue == '0') {
    if (_tokensTaken < 0) // I'm getting wrecked
      res += _tokensTaken * (30 + 5 * _blackScore);
    else
      res += _tokensTaken * (30 + 5 * _whiteScore);
  }
  else if (_colorValue == '1') {
    if (_tokensTaken < 0)
      res += _tokensTaken * (30 + 5 * _whiteScore);
    else
      res += _tokensTaken * (30 + 5 * _blackScore);
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

#include <ctime>
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
	clock_t tStart = clock();
	computesMinMax(TREE_DEPTH, _colorValue, ABMIN, ABMAX);
	printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
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
  static int xplayed = -1;
  static int yplayed = -1;
  if (xplayed != _best_x1 || yplayed != _best_y1) {
    _best_x = _best_x1;
    _best_y = _best_y1;
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
