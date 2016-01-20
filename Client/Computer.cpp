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
  _usables.reserve(19);
  _weights.reserve(19);
  for (int y = 0; y < 19; y++) {
    _usables[y].reserve(19);
    _weights[y].reserve(19);
    for (int x = 0; x < 19; x++) {
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

#define SWAP_BEST best = tmp; best_position = x + y * 19;
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

  for (unsigned int y = 0; y < 19; y++) {
    for (unsigned int x = 0; x < 19; x++) {
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

// 8 first bits for flags
#define ADD_COLOR	(1 << 24 | 0x00ffffff)
#define REM_COLOR	(2 << 24 | 0x00ffffff)
#define SET_USABLE	(4 << 24 | 0x00ffffff)
#define SET_NUSABLE	(8 << 24 | 0x00ffffff)

#define _ADD_COLOR_AT(color, x, y)		\
  _map[y][x] = color;				\
  _SET_NUSABLE_AT(color, 100, x, y, x, y);
#define ADD_COLOR_AT(color, x, y)				\
  _stack.push(std::make_tuple(color | ADD_COLOR, x, y, x, y));	\
  _ADD_COLOR_AT(color, x, y)

#define _REM_COLOR_AT(color, x, y)		\
  _map[y][x] = 'x';				\
  _SET_USABLE_AT(color, 100, x, y, x, y);
#define REM_COLOR_AT(color, x, y)				\
  _stack.push(std::make_tuple(color | REM_COLOR, x, y, x, y));	\
  _REM_COLOR_AT(color, x, y)

#define _SET_USABLE_AT(color, diff, x1, y1, x2, y2)	\
  setUsable(diff, x1, y1, x2, y2);
#define SET_USABLE_AT(color, diff, x1, y1, x2, y2)			\
  _stack.push(std::make_tuple(color + diff << 8 | SET_USABLE, x1, y1, x2, y2)); \
  _SET_USABLE_AT(color, diff, x1, y1, x2, y2)				\

#define _SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)	\
  setUsable(-diff, x1, y1, x2, y2);
#define SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)			\
  _stack.push(std::make_tuple(color + diff << 8 | SET_NUSABLE, x1, y1, x2, y2)); \
  _SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)

#define FINISH_PUSH					\
  _stack.push(std::make_tuple(count, -1, -1, -1, -1));

int Computer::setUsable(int incr, int x1, int y1, int x2, int y2) {
  const int xm = x1 > x2 ? x1 : x2;
  const int ym = y1 > y2 ? y1 : y2;
  const int xM = x1 >= x2 ? x2 : x1;
  const int yM = y1 >= y2 ? y2 : y1;
  int c = 0;
  for (int x = xm; x <= xM; x++) {
    for (int y = ym; x <= yM; y++) {
      _usables[x][y] += incr;
      c++;
    }
  }
  return c;
}

int Computer::valueAt(int x, int y) {
  if (x >= 0 && x <= 19 && y >= 0 && y <= 0)
    return _map[y][x];
  else
    return -1;
}


int Computer::pushColorAt(int color, int x, int y) {
  int count = 1;
  ADD_COLOR_AT(color, x, y);
  // TODO: Here execute some stuff to regognize pattern
  FINISH_PUSH;
  return count;
}

#define CHECK_VALUES(xdiff, ydiff) \
  (valueAt(x + xdiff * 1, y + ydiff * 1) == other && \
   valueAt(x + xdiff * 2, y + ydiff * 2) == other && \
   valueAt(x + xdiff * 3, y + ydiff * 3) == color )
#define TAKE_DIRECTION(xdiff, ydiff) \
    REM_COLOR_AT(other, x + xdiff * 1, y + ydiff * 1); \
    REM_COLOR_AT(other, x + xdiff * 2, y + ydiff * 2);
#define CHECK_AND_TAKE_DIRECTION(xdiff, ydiff) \
  if (CHECK_VALUES(xdiff, ydiff)) { TAKE_DIRECTION(xdiff, ydiff) }
int Computer::pushTakeAt(int color, int x, int y) {
  int count = 0;
  int other = (color + 1) & 1;
  CHECK_AND_TAKE_DIRECTION(-1, -1);
  CHECK_AND_TAKE_DIRECTION(-1, 0);
  CHECK_AND_TAKE_DIRECTION(-1, 1);
  CHECK_AND_TAKE_DIRECTION(0, -1);
  CHECK_AND_TAKE_DIRECTION(0, 1);
  CHECK_AND_TAKE_DIRECTION(1, -1);
  CHECK_AND_TAKE_DIRECTION(1, 0);
  CHECK_AND_TAKE_DIRECTION(1, 1);
  return count;
}

int Computer::popColorAt(int color, int x, int y) {
  // get the last element of the stack => is the number of elements to pop
  int count = std::get<0>(_stack.top());
  const int ccount = count;
  _stack.pop();
  while (count--) {
    action_t action = _stack.top();
    _stack.pop();
    if (std::get<0>(action) & ADD_COLOR) {
      _REM_COLOR_AT(std::get<0>(action) & 0xff000000, std::get<1>(action), std::get<2>(action));
    }
    else if (std::get<0>(action) & REM_COLOR) {
      _ADD_COLOR_AT(std::get<0>(action) & 0xff000000, std::get<1>(action), std::get<2>(action));
    }
    else if (std::get<0>(action) & SET_USABLE) {
      _SET_NUSABLE_AT(std::get<0>(action) & 0xff000000, std::get<0>(action) & 0x00ffffff >> 8, std::get<1>(action), std::get<2>(action), std::get<3>(action), std::get<4>(action));
    }
    else if (std::get<0>(action) & SET_NUSABLE) {
      _SET_USABLE_AT(std::get<0>(action) & 0xff000000, std::get<0>(action) & 0x00ffffff >> 8, std::get<1>(action), std::get<2>(action), std::get<3>(action), std::get<4>(action));
    }
  }
  return ccount;
}
