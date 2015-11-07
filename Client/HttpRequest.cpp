#include "HttpRequest.hpp"

HttpRequest::HttpRequest(std::string const & url,
			 std::string const & host,
			 std::string const & verb) {
  _query = verb + " " + url + "HTTP/1.0\r\n";
  _query += "Host: " + host + "\r\n";
  _query += "Accept: */*\r\n";
}

HttpRequest::~HttpRequest() {}

void HttpRequest::addCookie(std::string const & name, std::string const & value) {
  _query += "Cookie: " + name + ":=" + value + "\r\n";
}

std::string HttpRequest::toString() const {
  return _query;
}
