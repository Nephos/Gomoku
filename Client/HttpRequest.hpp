#pragma once

#include <string>
#include "Network.hpp"

class HttpRequest {
public:
  HttpRequest(std::string const & url,
	      std::string const & host,
	      std::string const & verb = "GET");
  ~HttpRequest();
  void addCookie(std::string const & name, std::string const & value);
  std::string toString() const;

protected:
  std::string _query;
};
