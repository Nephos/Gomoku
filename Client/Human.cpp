#include <sstream>
#include "Human.hpp"

Human::Human(std::string const &host, std::string const &port,
              std::string const &cookie, std::string const &color, bool rainbows)
              : Player(host, port, cookie, color), _display(rainbows) {}

Human *Human::p = NULL;
Human *Human::getInstance(std::string const &host, std::string const &port,
                            std::string const &cookie, std::string const &color, bool rainbows) {
  if (p == NULL)
    p = new Human(host, port, cookie, color, rainbows);
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
    click.first = -1;
    click.second = -1;
    ans = _network.getAnswer();
    parseAnswer(ans);
    click = _display.drawGame(_map);
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

void Human::sendClick(std::pair<int, int> click, std::string const &header) {
  std::stringstream ss;
  ss << "POST /game/play/" << click.first << "/" << click.second << header << _cookie << "\r\n\r\n";
  std::string req = ss.str();
  _network.sendQuery(req);
  _network._io_service.run();
  _network._io_service.reset();
  std::string ans = _network.getAnswer();
  parseAnswer(ans);
  req = "GET /game/map.txt" + header + _cookie + "\r\n\r\n";
  _network.sendQuery(req);
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

std::pair<int, int> Human::updateMap(std::istringstream &ss) {
  std::pair<int, int> score = Player::updateMap(ss);
  _display.setWhiteScore(score.first);
  _display.setBlackScore(score.second);
  return score;
}
