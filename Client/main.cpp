#include <iostream>
#include "Exceptions.hpp"
#include "Player.hpp"

int main(int ac, char **av) {
  try {
    if (ac >= 1) {
      std::string host, port;
      if (ac == 2) {
        host.assign(av[2]);
        unsigned int sep = host.find(":");
        if (host.find(":") == std::string::npos || sep + 1 == host.length())
          throw Gomoku::UsageException();
        port = host.substr(sep + 1);
        host = host.substr(0, sep);
      }
      if (host.size() == 0 || port.size() == 0)
        Player::getInstance()->play();
      else
        Player::getInstance(host, port)->play();
    }
    else
      throw Gomoku::UsageException();
  }
  catch (const Gomoku::GomokuException &e) {
    std::cout << "Game Error ! " << e.what() << std::endl;
  }
  catch (const std::exception &e) {
    std::cout << "Runtime Error ! " << e.what() << std::endl;
  }
}
