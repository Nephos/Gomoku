#pragma once

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <stack>
#include "Player.hpp"

void calc_initial_weight(char map[19][19]);
unsigned char *getWeightsMap();
void update_tile(char map[19][19], int n);
#define TREE_WEIGHT 5
#define TREE_DEEPTH 2

class Computer : public Player
{
public:
  void play();

  static Computer *getInstance(std::string const &host = "127.0.0.1", std::string const &port = "8080");

  bool parseAnswer(std::string const &);
  void resetGame();
  int initializeMinMax();
  int computesMinMax(int deepth_max, int current_color);
  int pushColorAt(int color, int x, int y);
  int pushTakeAt(int color, int x, int y);
  int popColorAt(int color, int x, int y);
  int setUsable(int incr, int, int, int, int);
  int valueAt(int x, int y);
  void setRandomBestPosition();
  void setMoveToXY(int color, int x, int y);

private:
  Computer(std::string const &, std::string const &);
  ~Computer() {};
  static Computer *p;

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
  int _tree_x, _tree_y;
};
