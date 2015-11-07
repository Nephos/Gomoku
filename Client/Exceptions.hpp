#pragma once

#include <exception>
#include <string>
#include <iostream>

namespace Gomoku {
  class GomokuException : public std::exception {
  protected:
    std::string const _str;
    std::string const _type;

  public:
    GomokuException(std::string const &str, std::string const &type = "") : std::exception(), _str(str), _type(type) {};
    ~GomokuException() throw() {};
    const char *what() const throw() { return (_type + _str).c_str(); };
  };

  class UsageException : public GomokuException {
  public:
    UsageException() : GomokuException("./client [ host:port ]", "Usage: ") {};
  };

  class NetworkException : public GomokuException {
  public:
    NetworkException(std::string const &str) : GomokuException(str, "Network error: ") {};
  };
}
