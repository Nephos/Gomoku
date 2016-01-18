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
  _weights.reserve(_map.size());
  _usables.reserve(_map.size());
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

// Add the 'current_color' to the '_stack' and '_stack'
#define UPDATE ;
#define COMPUTES_HEURISTIC ;

#define SWAP_BEST best = tmp; best_position = tmp_position;
#define SWAP_BEST_IF(cond) if (cond) { SWAP_BEST }
#define GO_DEEPER (current_color == getColor() ? computesMinMax(deepth_max-1, (current_color + 1) % 2) : computesMinMax(deepth_max-1, (current_color + 1) % 2));

#define weights _weights

int Computer::computesMinMax(int deepth_max, int current_color) {
  // pour toutes les cases vides qui ne nous font pas perdre
  // 'best' is the value of the better place
  int best = 0, best_position = -1;
  // 'tmp' is the value of the current place, to compare with 'best'
  int tmp, tmp_position;
  int player_color = getColor();

  for (unsigned int y = 0; y < _map.size(); y++) {
    for (unsigned int x = 0; x < _map.size(); x++) {
      tmp_position = x + y * _map.size();
      if (_usables[tmp_position] == false)
        continue;

      UPDATE;
      COMPUTES_HEURISTIC;

      tmp = weights[tmp_position];

      if (deepth_max > 1) {
        tmp_position = GO_DEEPER;
      }

      if (player_color == current_color) {
        SWAP_BEST_IF(tmp >= best);
      }
      else {
        SWAP_BEST_IF(tmp <= best);
      }
    }
  }
  if (best_position == -1) {
    return _map.size() * _map.size() / 2;
  }
  return best_position;
}
