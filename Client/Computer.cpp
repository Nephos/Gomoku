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
      _network.sendQuery(req);
    }
    else if (!_gameOver) {
      ;// Play & send move to network
    }
  }
}

/* If returns false, then ask for another user input */
/* Else, wait for the other client */
bool Computer::parseAnswer(const std::string &str) {
  if (!Player::parseAnswer(str))
    return false;
/* Different possible answers impacting the game
  if (_gameOver && _win)

  else if (_gameOver && !_win)

  else if (_myTurn)

  else if (!_myTurn)
*/
  return true;
}

int Computer::initializeMinMax() {
  _usables.reserve(_map.size());
  _weights.reserve(_map.size());
  for (int y = 0; y < _map.size(); y++) {
    _usables[y].reserve(_map.size());
    _weights[y].reserve(_map.size());
    for (int x = 0; x < _map.size(); x++) {
      _usables[y][x] = false;
      _weights[y][x] = 0;
    }
  }
  return 0;
}

// TODO
// Add the 'current_color' to the '_stack' and '_stack'
#define UPDATE pushColorAt(current_color, x, y);
// TODO
// Remove infos from the stack
#define RESET popColorAt(current_color, x, y);
// TODO
// Calculate weights with the current infos
#define COMPUTES_HEURISTIC

#define EVALUATES_MAP 0

#define SWAP_BEST best = tmp; best_position = x + y * _map.size();
#define SWAP_BEST_IF(cond) if (cond) { SWAP_BEST }

// TODO
// get the better values from the deeper trees
#define GO_DEEPER computesMinMax(deepth_max - 1, (current_color + 1) & 1, !self_turn)

/*
 * If on a leaf, calc the heuristic
 * If not, for each interesting usables (x, y) go deeper and keep only the max/min
 */
int Computer::computesMinMax(int deepth_max, int current_color, bool self_turn) {
  // pour toutes les cases vides qui ne nous font pas perdre
  // 'best' is the value of the better place
  int best = 0, best_position = -1;
  // 'tmp' is the value of the current place, to compare with 'best'
  int tmp, tmp_position;

  // if (LOOSE) return -100;
  // if (WIN) return 100;
  // evaluate the state and return it
  if (deepth_max == 0) {
    return EVALUATES_MAP;
  }

  for (unsigned int y = 0; y < _map.size(); y++) {
    for (unsigned int x = 0; x < _map.size(); x++) {
      if (_usables[y][x] == false)
        continue;

      UPDATE; // that push on _stack
      tmp = GO_DEEPER;
      RESET; // that pop from _stack

      if (self_turn) {
        SWAP_BEST_IF(tmp >= best || best_position == -1);
      }
      else {
        SWAP_BEST_IF(tmp <= best || best_position == -1);
      }
    }
  }
  //TODO: keep best_position in an attribute ? _best_position = best_position;
  return best;
}

#define SELF		0
#define OPNT		1

#define ADD_COLOR	2
#define SET_USABLE	4
#define SET_NUSABLE	8

#define _SET_USABLE_AT(color, x1, y1, x2, y2, v)			\
  _stack.push(std::make_tuple(color & SET_USABLE, x1, y1, x2, y2))	\
  setUsable(true, x1, y1, x2, y2);
#define _SET_NUSABLE_AT(color, x1, y1, x2, y2, v)			\
  _stack.push(std::make_tuple(color & SET_NUSABLE, x1, y1, x2, y2));	\
  setUsable(false, p1, p2);
#define _ADD_COLOR_AT(color, x, y)				\
  _stack.push(std::make_tuple(color & ADD_COLOR, x, y, x, y));	\
  // TODO							\
  //_map[y][x] = color;						\
  _SET_USABLE_AT(color, x, y, x, y, v);
#define FINISH_PUSH				\
  _stack.push(std::make_tuple(count, -1, -1, -1, -1));

int Computer::setUsable(bool v, int x1, int y1, int x2, int y2) {
  const int xm = x1 > x2 ? x1 : x2;
  const int ym = y1 > y2 ? y1 : y2;
  const int xM = x1 >= x2 ? x2 : x1;
  const int yM = y1 >= y2 ? y2 : y1;
  int c = 0;
  for (int x = xm; x <= x1; x++) {
    for (int y = ym; x <= y1; y++) {
      // TODO
      // if (_map[x][y] != 0) {
      // 	_usables[x][y] = v;
      // 	c++;
      // }
    }
  }
  return c;
}

int Computer::pushColorAt(int color, int x, int y) {
  int count = 1;
  _ADD_COLOR_AT(color, x, y);
  // Here execute some stuff to regognize pattern
  FINISH_PUSH;
  return count;
}

int Computer::popColorAt(int color, int x, int y) {
  int count = std::get<0>(_stack.top()); // get the last element of the stack which is the number of elements to pop
  const int ccount = count;
  _stack.pop();
  while (count--) {
    action_t action = _stack.top();
    _stack.pop();
    // execute the invert of 'action'
  }
  return ccount;
}
