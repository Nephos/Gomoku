#include <sstream>
#include "Human.hpp"

Human::Human(std::string const &host, std::string const &port, bool rainbows, bool rotate)
              : Player(host, port), _display(rainbows, rotate) {}

Human *Human::p = NULL;
Human *Human::getInstance(bool rainbows, bool rotate, std::string const &host, std::string const &port) {
  if (p == NULL)
    p = new Human(host, port, rainbows, rotate);
  return p;
}

void Human::resetGame() {
  Player::resetGame();
  _display.setWhiteScore(0);
  _display.setBlackScore(0);
  _display.setMessage("Waiting for other players...");
}

void Human::play() {
  Player::play();
  std::pair<int, int> click(-1, -1);
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  _display.setMessage("Waiting for other players...");
  while (click.first != -2) { // Game loop
    _display.setWhiteScore(_whiteScore);
    _display.setBlackScore(_blackScore);
    click.first = -1;
    click.second = -1;
    ans = _network.getAnswer();
    parseAnswer(ans);
    click = _display.drawGame(_map);
    if (_display.getRulesChanged()) {
      std::string req = "GET /game/options/break5/" + (_display.getBreak() ? std::string("true") : std::string("false")) + header + _cookie + "\r\n\r\n";
      _network.sendQuery(req);
      req = "GET /game/options/free3/" + (_display.getBreak() ? std::string("true") : std::string("false")) + header + _cookie + "\r\n\r\n";
      _network.sendQuery(req);
    }
    if (click.first == -3)
      resetGame();
    else if (!_myTurn && !_gameOver) {
      std::string req = "GET /game.txt" + header + _cookie + "\r\n\r\n";
      _network.sendQuery(req);
    }
    else if (!_gameOver && click.first >= 0 && click.second >= 0)
      sendClick(click, header);
  }
}

/* If returns false, then ask for another user input */
/* Else, wait for the other client */
bool Human::parseAnswer(const std::string &str) {
  if (!Player::parseAnswer(str))
    return false;
  if (_gameOver && _win)
    _display.setMessage("Game over, you win. Press ESC to quit or SPACE to play again.");
  else if (_gameOver && !_win)
    _display.setMessage("Game over, you lose. Press ESC to quit or SPACE to play again.");
  else if (_myTurn)
    _display.setMessage("It's your turn !");
  else if (!_myTurn)
    _display.setMessage("It's the enemy's turn !");
  _display.setColor(_color);
  return true;
}
