#include <sstream>
#include "Player.hpp"

Player::Player(std::string const &host, std::string const &port) : _network(host, port) {
  _myTurn = false;
  _host = host + ":" + port;
}

Player *Player::p = NULL;
Player *Player::getInstance(std::string const &host, std::string const &port) {
  if (p == NULL)
    p = new Player(host, port);
  return p;
}

void Player::connect() {
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  std::string *str = new std::string("GET /players/connect" + header + "\r\n");
  _network.sendQuery(str);
}

void Player::play() {
  connect();
  std::string ans;
  std::string header = " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n";
  for (int i = 0; i < 2; i++) { // Game loop
    ans = _network.getAnswer();
    parseAnswer(ans);
    if (!_myTurn) {
      std::string *req = new std::string("GET /game" + header + _cookie + "\r\n\r\n");
      std::cout << *req << std::endl;
      _network.sendQuery(req);
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
  std::istringstream ss(str);
  std::string tmp;
  bool setCookie = false;
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
  return true;
}