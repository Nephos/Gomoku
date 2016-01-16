#include <iostream>
#include "Exceptions.hpp"
#include "Player.hpp"
#include "Human.hpp"

int main(int ac, char **av) {
  try {
    if (ac >= 1) {
      std::string host, port, cookie, color;
      if (ac >= 2) {
        host.assign(av[1]);
        unsigned int sep = host.find(":");
        if (host.find(":") == std::string::npos || sep + 1 == host.length())
          throw Gomoku::UsageException();
        port = host.substr(sep + 1);
        host = host.substr(0, sep);
      }
      if (ac >= 4) {
        cookie.assign(av[2]);
        color.assign(av[3]);
      }
      if (host.size() == 0 || port.size() == 0)
        Human::getInstance()->play();
      else
        Human::getInstance(host, port, cookie, color)->play();
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
