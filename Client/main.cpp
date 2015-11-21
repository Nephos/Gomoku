#include <iostream>
#include "Exceptions.hpp"
#include "Player.hpp"

int main(int ac, char **av) {
  try {
    std::string host, port;
    bool rainbow = true;
    if (ac == 2) {
      host.assign(av[1]);
      unsigned int sep = host.find(":");
      if (host.find(":") == std::string::npos || sep + 1 == host.length()) {
        if (host.compare("--norainbowsplease") == 0)
          rainbow = false;
        else
          throw Gomoku::UsageException();
      }
      else {
        port = host.substr(sep + 1);
        host = host.substr(0, sep);
      }
    }
    if (ac == 3) {
      std::string r(av[2]);
      if (r.compare("--norainbowsplease") == 0)
        rainbow = false;
      else
        throw Gomoku::UsageException();
    }
    if (host.size() == 0 || port.size() == 0)
      Player::getInstance(rainbow)->play();
    else
      Player::getInstance(rainbow, host, port)->play();
  }
  catch (const Gomoku::GomokuException &e) {
    std::cout << "Game Error ! " << e.what() << std::endl;
  }
  catch (const std::exception &e) {
    std::cout << "Runtime Error ! " << e.what() << std::endl;
  }
}
