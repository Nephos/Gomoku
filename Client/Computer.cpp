#include <sstream>
#include "Computer.hpp"

Computer::Computer(std::string const &host, std::string const &port,
              std::string const &cookie, std::string const &color)
              : Player(host, port, cookie, color) {}

Computer *Computer::p = NULL;
Computer *Computer::getInstance(std::string const &host, std::string const &port,
                            std::string const &cookie, std::string const &color) {
  if (p == NULL)
    p = new Computer(host, port, cookie, color);
  return p;
}

void Computer::resetGame() {
  // Reset les poids / l'IA ici
}

void Computer::play() {
  Player::play();
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
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

std::pair<int, int> Computer::updateMap(std::istringstream &ss) {
  std::pair<int, int> score = Player::updateMap(ss);
  /* Update les poids ici */
  return score;
}
