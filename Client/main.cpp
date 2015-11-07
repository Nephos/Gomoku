#include <iostream>
#include "Exceptions.hpp"
#include "Player.hpp"

int main(int ac, char **av) {
  try {
    if (ac >= 2) {
      std::string colour(av[1]);
      if (colour.compare("black") != 0 && colour.compare("white") != 0)
        throw Gomoku::UsageException();
      std::string host, port;
      if (ac == 3) {
        host.assign(av[2]);
        unsigned int sep = host.find(":");
        if (host.find(":") == std::string::npos || sep + 1 == host.length())
          throw Gomoku::UsageException();
        port = host.substr(sep + 1);
        host = host.substr(0, sep);
      }
      if (host.size() == 0 || port.size() == 0)
        Player::getInstance(colour)->play();
      else
        Player::getInstance(colour, host, port)->play();
    }
    else
      throw Gomoku::UsageException();
  }
  catch (const Gomoku::GomokuException &e) {
    std::cout << e.what() << std::endl;
  }
}