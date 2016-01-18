#include <iostream>
#include "Exceptions.hpp"
#include "Computer.hpp"
#include "Human.hpp"

int main(int ac, char **av) {
  try {
    if (ac >= 1) {
      std::string host, port;
      bool rainbows, player;
      rainbows = player = true;
      if (ac >= 2) {
        host.assign(av[1]);
        unsigned int sep = host.find(":");
        if (host.find(":") == std::string::npos || sep + 1 == host.length()) {
          host = "";
          if (std::string(av[1]).compare("--norainbowsplease") == 0)
            rainbows = false;
          else if (std::string(av[1]).compare("--ia=true") == 0)
            player = false;
          else
            throw Gomoku::UsageException();
        }
        else {
          port = host.substr(sep + 1);
          host = host.substr(0, sep);
        }
      }
      if (ac >= 3 && std::string(av[2]).compare("--norainbowsplease") == 0)
        rainbows = false;
      else if (ac >= 3 && std::string(av[2]).compare("--ia=true") == 0)
        player = false;
      if (player && (host.size() == 0 || port.size() == 0))
        Human::getInstance(rainbows)->play();
      else if (player)
        Human::getInstance(rainbows, host, port)->play();
      else if (host.size() == 0 || port.size() == 0)
        Computer::getInstance()->play();
      else
        Computer::getInstance(host, port)->play();
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
