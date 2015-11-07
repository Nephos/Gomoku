#include "Player.hpp"

Player::Player(std::string const &colour, std::string const &host, std::string const &port) : _network() {
  _colour = colour;
  _host = host + ":" + port;
  _network.connect(host, port);
}

Player *Player::p = NULL;
Player *Player::getInstance(std::string const &colour, std::string const &host, std::string const &port) {
  if (p == NULL)
    p = new Player(colour, host, port);
  return p;
}

void Player::play() {
  std::string *req = new std::string("GET /players/connect/" + _colour);
  *req += " HTTP/1.0\r\nHost: " + _host + "\r\nAccept: */*\r\n\r\n";
  _network.sendQuery(req);
  _network.update();
}