#pragma once

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <stack>
#include "Player.hpp"

#define TREE_WEIGHT 128
#define TREE_DEPTH 4

#define ABMIN 0b10000000000000000000000000000000
#define ABMAX 0b01111111111111111111111111111111

class Computer : public Player
{
public:
  void play();

  static Computer *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080");

  bool parseAnswer(std::string const &);
  void resetGame();
  int initializeMinMax();
  int computesMinMax(int depth_max, int current_color, int, int);
  int pushColorAt(int color, int x, int y);
  int pushTakeAt(int color, int x, int y);
  int popColorAt(int color, int x, int y);
  int setUsable(int incr, int, int, int, int);
  int valueAt(int x, int y);
  void setRandomBestPosition();
  void setMoveToXY(int color, int x, int y);
  int computeHeuristic();
  int checkVictory();

private:
  Computer(std::string const &, std::string const &);
  ~Computer() {};
  static Computer *p;
  int _tokensTaken;

#define line(T) std::vector<T >
  line(line(int)) _weights;

  // true / false if tile is usable ... TODO: more data
  line(line(int)) _usables;

  // <action, x1, y1, x2, y2 >
  typedef std::tuple<int, int, int, int, int> action_t;
  std::stack<std::tuple<int, int, int, int, int> > _stack;

  // <empreinte, <map, weights> >
  std::map<long long, line(line(int)), line(line(int)) > _cache;

  // tree search
  int _best_x, _best_y;
  int _best_x1, _best_y1;
  int _tree_x, _tree_y;

  int _taken_diff[2];
};
