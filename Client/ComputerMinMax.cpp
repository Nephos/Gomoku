#include <iostream>
#include "Computer.hpp"

int Computer::initializeMinMax() {
  std::get<0>(_lastMove) = -1;
  std::get<1>(_lastMove) = -1;
  for (int y = 0; y < 19; y++) {
    std::vector<int> tmpi;
    std::vector<int> tmpb;
    for (int x = 0; x < 19; x++) {
      tmpi.push_back(0);
      tmpb.push_back(0);
    }
    _usables.push_back(tmpb);
    _weights.push_back(tmpi);
  }
  return 0;
}

int Computer::checkVictory() {
  // Checking in each direction from the tile we put
  static std::pair<int, int> dirs[] = {
    {-1, -1}, // North West
    {0, -1}, // North... I think ?
    {1, -1}, // North East
    {1, 0}, // East I guess
  };

  for (int dir = 0; dir < 4; dir++) {
    // Dir in one way and then in the other
    // Looking for lines here...
    int line = 1;
    int free = 0;

    for (int n = 1; n < 5; n++) {
      int tmpx = _tree_x + dirs[dir].first * n;
      int tmpy = _tree_y + dirs[dir].second * n;
      if (tmpx < 0 || tmpx > 18
        || tmpy < 0 || tmpy > 18)
        break; // Out of bounds

      if (_map[tmpy][tmpx] != _map[_tree_y][_tree_x]) {
        if (_map[tmpy][tmpx] == 'x')
          free++;
        break; // End of pattern
      }
      line++;
    }
    // In the other way
    for (int n = 1; n < 5; n++) {
      int tmpx = _tree_x + -dirs[dir].first * n;
      int tmpy = _tree_y + -dirs[dir].second * n;
      if (tmpx < 0 || tmpx > 18
        || tmpy < 0 || tmpy > 18)
        break; // Out of bounds

      if (_map[tmpy][tmpx] != _map[_tree_y][_tree_x]) {
        if (_map[tmpy][tmpx] == 'x')
          free++;
        break; // End of pattern
      }
      line++;
    }
    if (line < 2)
      continue;
    if (_map[_tree_y][_tree_x] == _colorValue + '0') {
      if (line == 5)
        return 1000;
    }
    else {
      if (line == 5)
        return -1000;
    }
  }
  return 0;
}

/*
 * If on a leaf, calc the heuristic
 * If not, for each interesting usables (x, y) go deeper and keep only the max/min
 */
int Computer::computesMinMax(int deepth_max, int current_color) {
  // pour toutes les cases vides qui ne nous font pas perdre
  // 'best' is the value of the better place
  int best = (current_color == _colorValue ? (~0) : ((~0 << 1) >> 1)); // -inf or +inf
  // 'tmp' is the value of the current place, to compare with 'best'
  int tmp, tmp_position;
  int count = 0;
  bool is_self_turn = current_color == _colorValue;

  // if (LOOSE) return -100;
  // if (WIN) return 100;
  // evaluate the state and return it
  if (deepth_max == 0) {
    return computeHeuristic();
  }

  for (unsigned int y = 0; y < 19; y++) {
    for (unsigned int x = 0; x < 19; x++) {
      if (count > TREE_WEIGHT || _usables[y][x] <= 0)
        continue;

      count++;
      pushColorAt(current_color, x, y); // that push on _stack
      _tree_x = x;
      _tree_y = y;
      int victory = checkVictory();
      if (victory != 0) {
        if (deepth_max == TREE_DEEPTH) {
    	    _best_x = x;
    	    _best_y = y;
    	  }
        popColorAt(current_color, x, y);
        return victory;
      }
      tmp = computesMinMax(deepth_max - 1, current_color ^ 1);
      popColorAt(current_color, x, y); // that pop from _stack

      if (is_self_turn) {
        if (tmp >= best) {
	  best = tmp;
	  if (deepth_max == TREE_DEEPTH) {
	    _best_x = x;
	    _best_y = y;
	  }
	}
      }
      else {
        if (tmp <= best) {
	  best = tmp;
	}
      }
    }
  }
  return best;
} // after it, re-run pushColorAt(_colorValue, _best_position, ...)

// 8 first bits for flags
#define ADD_COLOR	(1 << 24)
#define REM_COLOR	(2 << 24)
#define SET_USABLE	(4 << 24)
#define SET_NUSABLE	(8 << 24)

#define _ADD_COLOR_AT(color, x, y)		\
  _map[y][x] = color + '0';
#define ADD_COLOR_AT(color, x, y)				\
  _stack.push(std::make_tuple(color | ADD_COLOR, x, y, x, y));	\
  count++;							\
  _ADD_COLOR_AT(color, x, y)
#define _REM_COLOR_AT(color, x, y)		\
  _map[y][x] = 'x';
#define REM_COLOR_AT(color, x, y)				\
  _stack.push(std::make_tuple(color | REM_COLOR, x, y, x, y));	\
  count++;							\
  _REM_COLOR_AT(color, x, y)
#define _SET_USABLE_AT(color, diff, x1, y1, x2, y2)	\
  setUsable(diff, x1, y1, x2, y2);
#define SET_USABLE_AT(color, diff, x1, y1, x2, y2)			\
  _stack.push(std::make_tuple(color | (diff << 8) | SET_USABLE, x1, y1, x2, y2)); \
  count++;								\
  _SET_USABLE_AT(color, diff, x1, y1, x2, y2)
#define _SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)	\
  setUsable(-diff, x1, y1, x2, y2);
#define SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)			\
  _stack.push(std::make_tuple(color | (diff << 8) | SET_NUSABLE, x1, y1, x2, y2)); \
  count++;								\
  _SET_NUSABLE_AT(color, diff, x1, y1, x2, y2)

#define CHECK_VALUES(xdiff, ydiff)			\
  (valueAt(x + xdiff * 1, y + ydiff * 1) == other &&	\
   valueAt(x + xdiff * 2, y + ydiff * 2) == other &&	\
   valueAt(x + xdiff * 3, y + ydiff * 3) == color )
#define TAKE_DIRECTION(xdiff, ydiff)					\
  REM_COLOR_AT(other, x + xdiff * 1, y + ydiff * 1);			\
  SET_USABLE_AT(color, 1000, x + xdiff * 1, y + ydiff * 1, x + xdiff * 1, y + ydiff * 1); \
  REM_COLOR_AT(other, x + xdiff * 2, y + ydiff * 2);			\
  SET_USABLE_AT(color, 1000, x + xdiff * 2, y + ydiff * 2, x + xdiff * 2, y + ydiff * 2);
#define CHECK_AND_TAKE_DIRECTION(xdiff, ydiff)				\
  if (CHECK_VALUES(xdiff, ydiff)) { TAKE_DIRECTION(xdiff, ydiff) }
#define CHECK_AND_TAKE_ALL_DIRECTION		\
  CHECK_AND_TAKE_DIRECTION(-1, -1);		\
  CHECK_AND_TAKE_DIRECTION(-1, 0);		\
  CHECK_AND_TAKE_DIRECTION(-1, 1);		\
  CHECK_AND_TAKE_DIRECTION(0, -1);		\
  CHECK_AND_TAKE_DIRECTION(0, 1);		\
  CHECK_AND_TAKE_DIRECTION(1, -1);		\
  CHECK_AND_TAKE_DIRECTION(1, 0);		\
  CHECK_AND_TAKE_DIRECTION(1, 1);

#define ADD_3FREE				\
  0

int Computer::pushColorAt(int color, int x, int y) {
#ifdef DEBUG
  std::cout << "Push color (" << color << ") at (" << x << ":" << y << ")"  << std::endl;
#endif //DEBUG
  int count = 0;
  ADD_COLOR_AT(color, x, y);
  SET_NUSABLE_AT(color, 1000, x, y, x, y);
  SET_USABLE_AT(color, 5, x-1, y-1, x+1, y+1); // radius 1 = +10
  // SET_USABLE_AT(color, 5, x-2, y-2, x+2, y+2); // radius 2 = +5
  int other = color ^ 1;
  CHECK_AND_TAKE_ALL_DIRECTION;
  if (color == _colorValue) {
    ADD_3FREE;
  }
  _stack.push(std::make_tuple(count, -1, -1, -1, -1));
  return count;
}

int Computer::popColorAt(int color, int x, int y) {
  // get the last element of the stack => is the number of elements to pop
#ifdef DEBUG
  std::cout << "Pop color (" << color << ") at (" << x << ":" << y << ")"  << std::endl;
#endif //DEBUG
  int count = std::get<0>(_stack.top());
  const int ccount = count;
  _stack.pop();
  while (count--) {
    action_t action = _stack.top();
    _stack.pop();
    if (std::get<0>(action) & ADD_COLOR) {
      _REM_COLOR_AT(std::get<0>(action),
		    std::get<1>(action), std::get<2>(action));
    }
    else if (std::get<0>(action) & REM_COLOR) {
      _ADD_COLOR_AT(std::get<0>(action),
		    std::get<1>(action), std::get<2>(action));
    }
    else if (std::get<0>(action) & SET_USABLE) {
      _SET_NUSABLE_AT(std::get<0>(action),
		      ((std::get<0>(action) & 0x00ffff00) >> 8),
		      std::get<1>(action), std::get<2>(action),
		      std::get<3>(action), std::get<4>(action));
    }
    else if (std::get<0>(action) & SET_NUSABLE) {
      _SET_USABLE_AT(std::get<0>(action),
		     ((std::get<0>(action) & 0x00ffff00) >> 8),
		     std::get<1>(action), std::get<2>(action),
		     std::get<3>(action), std::get<4>(action));
    }
  }
  return ccount;
}

#define INTMAX(i, M) (i > M ? M : i)
#define INTMIN(i, m) (i < m ? m : i)
int Computer::setUsable(int incr, int x1, int y1, int x2, int y2) {
#ifdef DEBUG
  std::cout << "1. Set usable ( " << incr << ") pour " << x1 << " <= x <= " << x2 << " ET " << y1 << " <= y <= " << y2 << std::endl;
#endif //DEBUG
  x1 = INTMIN(INTMAX(x1, 18), 0);
  y1 = INTMIN(INTMAX(y1, 18), 0);
  x2 = INTMIN(INTMAX(x2, 18), 0);
  y2 = INTMIN(INTMAX(y2, 18), 0);
  const int xm = x1 < x2 ? x1 : x2;
  const int ym = y1 < y2 ? y1 : y2;
  const int xM = x1 <= x2 ? x2 : x1;
  const int yM = y1 <= y2 ? y2 : y1;
#ifdef DEBUG
  std::cout << "2. Set usable ( " << incr << ") pour " << xm << " <= x <= " << xM << " ET " << ym << " <= y <= " << yM << std::endl;
#endif //DEBUG
  int c = 0;
  for (int y = ym; y <= yM; y++) {
    for (int x = xm; x <= xM; x++) {
      _usables[y][x] += incr;
      c++;
    }
  }
#ifdef DEBUG
  std::cout << "Case modifiees : " << c << std::endl;
#endif //DEBUG
  return c;
}

int Computer::valueAt(int x, int y) {
  if (x >= 0 && x < 19 && y >= 0 && y < 19)
    return _map[y][x];
  else
    return -1;
}
