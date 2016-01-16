#include <sstream>
#include "Player.hpp"

Player::Player(std::string const &host, std::string const &port,
              std::string const &cookie, std::string const &color) : _network(host, port) {
  _cookie = cookie;
  _color = color;
  _myTurn = false;
  _host = host + ":" + port;
  initMap();
}

void Player::connect() {
  std::string str = "GET /players/connect/ HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n\r\n";
  _network.sendQuery(str);
}

void Player::initMap() {
  _map.clear();
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      std::pair<int, int> p(i, j);
      std::pair<std::pair<int, int>, char> e(p, 'x');
      _map.insert(e);
    }
  }
}

void Player::resetGame() {
  _cookie.clear();
  _gameOver = false;
  _myTurn = false;
  initMap();
  _network.reset();
  connect();
}

void Player::play() {
  connect();
  _gameOver = false;
}

/* If returns false, then ask for another user input */
/* Else, wait for the other client */
bool Player::parseAnswer(const std::string &str) {
  if (str.empty())
    return true;
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
        _win = false;
        updateMap(ss);
      }
      else if (tmp.find("win.") == 0) {
        _gameOver = true;
        _win = true;
        updateMap(ss);
      }
      else if (tmp.find("continue.") == 0 ||
        tmp.find("ok.") == 0) {
        if (tmp.find("continue") == 0) {
          if (!_myTurn) {
            _myTurn = true;
          }
          else {
            _myTurn = false;
          }
        }
        updateMap(ss);
      }
    }
  }
  setCookie(str);
  return true;
}

std::pair<int, int> Player::updateMap(std::istringstream &ss) {
  std::string tmp;
  _map.clear();
  int i = 0;
  while (i < 19 && std::getline(ss, tmp)) {
    for (int j = 0; j < 19; j++) {
      std::pair<int, int> c(i, j); // Coordinates
      std::pair<std::pair<int, int>, char> e(c, tmp.at(j * 2));
      _map.insert(e);
    }
    ++i;
  }
  int w, b;
  ss >> w;
  ss >> b;
  return std::pair<int, int> (w, b);
}

void Player::setCookie(const std::string &str) {
  if (_cookie == "") {
    bool cookieSet = false;
    std::istringstream ss(str);
    std::string tmp;
    while (std::getline(ss, tmp)) {
      if (tmp.find("Set-Cookie:") != std::string::npos) {
        if (!cookieSet) {
          cookieSet = true;
          _cookie = "Cookie: ";
        }
        int n = tmp.find(";path=/");
        tmp = tmp.substr(12, n - 12);
        _cookie += tmp + "; ";
        if (tmp.find("color") == 0) {
          _color = tmp.substr(6);
        }
      }
    }
  }
}
