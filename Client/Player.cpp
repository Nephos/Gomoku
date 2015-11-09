#include <sstream>
#include "Player.hpp"

Player::Player(std::string const &host, std::string const &port) : _network(host, port), _display() {
  _myTurn = false;
  _host = host + ":" + port;
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      std::pair<int, int> p(i, j);
      std::pair<std::pair<int, int>, char> e(p, 'x');
      _map.insert(e);
    }
  }
}

Player *Player::p = NULL;
Player *Player::getInstance(std::string const &host, std::string const &port) {
  if (p == NULL)
    p = new Player(host, port);
  return p;
}

void Player::connect() {
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  std::string str = "GET /players/connect/" + header + "\r\n";
  _network.sendQuery(str);
}

void Player::play() {
  connect();
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  std::pair<int, int> click(-1, -1);
  _gameOver = false;
  while (click.first != -2) { // Game loop
    ans = _network.getAnswer();
    parseAnswer(ans);
    click = _display.drawGame(_map);
    if (_gameOver && click.first == -3) {
      _cookie = ""; // resetting cookies to play a new game
      connect();
    }
    if (!_myTurn && !_gameOver) {
      std::string req = "GET /game.txt" + header + _cookie + "\r\n\r\n";
      _network.sendQuery(req);
    }
    else {
      if (click.first >= 0 && click.second >= 0) {
        std::stringstream ss;
        ss << "POST /game/play/" << click.first << "/" << click.second << header << _cookie << "\r\n\r\n";
        std::string req = ss.str();
        _network.sendQuery(req);
        _network._io_service.run();
        _network._io_service.reset();
        ans = _network.getAnswer();
        parseAnswer(ans);
        req = "GET /game/map.txt" + header + _cookie + "\r\n\r\n";
        _network.sendQuery(req);
      }
    }
  }
}

/* If returns false, then ask for another user input */
/* Else, wait for the other client */
bool Player::parseAnswer(const std::string &str) {
  if (str.find("401 Unauthorized") != std::string::npos)
    return false;
  else if (str.find("403 Forbidden") != std::string::npos) {
    connect();
    return false;
  }
  else {
    std::istringstream ss(str);
    std::string tmp;
    while (std::getline(ss, tmp)) {
      if (tmp.find("failed.") == 0) {
        _gameOver = true;
        _display.setMessage("Game over, you lose. Press ESC to quit or SPACE to play again.");
      }
      else if (tmp.find("win.") == 0) {
        _gameOver = true;
        _display.setMessage("Game over, you win. Press ESC to quit or SPACE to play again.");
      }
      else if (tmp.find("continue.") == 0 ||
        tmp.find("ok.") == 0) {
        if (tmp.find("continue") == 0) {
          _display.setMessage("It's your turn !");
          if (!_myTurn)
            _myTurn = true;
          else
            _myTurn = false;
        }
        /* We get the map */
        _map.clear();
        int i = 0;
        while (std::getline(ss, tmp)) {
          for (int j = 0; j < 19; j++) {
            std::pair<int, int> c(i, j); // Coordinates
            std::pair<std::pair<int, int>, char> e(c, tmp.at(j * 2));
            _map.insert(e);
          }
          ++i;
        }
      }
    }
  }
  if (_cookie == "") {
    bool setCookie = false;
    std::istringstream ss(str);
    std::string tmp;
    while (std::getline(ss, tmp)) {
      if (tmp.find("Set-Cookie:") != std::string::npos) {
        if (!setCookie) {
          setCookie = true;
          _cookie = "Cookie: ";
        }
        int n = tmp.find(";path=/");
        tmp = tmp.substr(12, n - 12);
        _cookie += tmp + "; ";
      }
    }
  }
  return true;
}