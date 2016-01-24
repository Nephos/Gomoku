#include <sstream>
#include "Player.hpp"

Player::Player(std::string const &host, std::string const &port) : _network(host, port) {
  _myTurn = false;
  _whiteScore = _blackScore = 0;
  _host = host + ":" + port;
  initMap();
}

int Player::getColor() const {
  return (_color == "white" ? 0 : 1);
}

void Player::connect() {
  std::string str = "GET /players/connect/ HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n\r\n";
  std::string ans = _network.sendSyncQuery(str);
  parseAnswer(ans);
}

void Player::sendClick(std::pair<int, int> click, std::string const &header) {
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

void Player::initMap() {
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      _map[i][j] = 'x';
    }
  }
}

void Player::resetGame() {
  _cookie.clear();
  _gameOver = false;
  _myTurn = false;
  _whiteScore = 0;
  _blackScore = 0;
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

void Player::updateMap(std::istringstream &ss) {
  std::string test = ss.str();
  std::string tmp;
  int i = 0;
  while (i < 19 && std::getline(ss, tmp)) {
    for (int j = 0; j < 19; j++) {
        _map[i][j] = tmp.at(j * 2);
    }
    ++i;
  }
  ss >> _whiteScore;
  ss >> _blackScore;
  std::getline(ss, tmp);
  ss >> std::get<0>(_lastMove);
  ss >> std::get<1>(_lastMove);
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
          _colorValue = getColor();
        }
      }
    }
  }
}
