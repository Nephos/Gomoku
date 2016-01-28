#include "Computer.hpp"

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
      bool empty = (_map[newy][newx] == 'x');
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
      res += _blackScore == 4 ? _tokensTaken * 1000 : _tokensTaken * (30 + 5 * _blackScore);
    else
      res += _whiteScore == 4 ? _tokensTaken * 1000 : _tokensTaken * (30 + 5 * _whiteScore);
  }
  else if (_colorValue == '1') {
    if (_tokensTaken < 0)
      res += _whiteScore == 4 ? _tokensTaken * 1000 : _tokensTaken * (30 + 5 * _whiteScore);
    else
      res += _blackScore == 4 ? _tokensTaken * 1000 : _tokensTaken * (30 + 5 * _blackScore);
  }

  return res;
}
