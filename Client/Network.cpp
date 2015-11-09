#include <cstring>
#include "Exceptions.hpp"
#include "Network.hpp"

Network::Network(std::string const &host, std::string const &port) : _resolver(_io_service), _sock(_io_service), _host(host), _port(port), _answered(true) {}

void Network::connect() {
  boost::asio::ip::tcp::resolver::query query(_host, _port);
  boost::asio::ip::tcp::resolver::iterator iterator = _resolver.resolve(query);
  boost::asio::connect(_sock.lowest_layer(), iterator);
}

void Network::handleRead(const boost::system::error_code &err, size_t bread) {
  _answered = true;
  if (err && err.value() != 2)
    throw Gomoku::NetworkException(err.message());
  if (bread > 0) {
    std::string tmp(_buff);
    _answers.push_back(tmp);
  }
  memset(_buff, '\0', max_length);
}

void Network::handleWrite(const boost::system::error_code &err) {
  if (err)
    throw Gomoku::NetworkException(err.message());
}

void Network::reset() {
  _io_service.poll();
  _io_service.reset();
  _answers.clear();
  _answered = true;
}

void Network::sendQuery(const std::string req) {
  _io_service.poll();
  _io_service.reset();
  if (_answered == 0)
    return;
  _answered = false;
  connect();
  const char *str = new char[req.length()];
  str = req.c_str();
  boost::asio::async_write(_sock, boost::asio::buffer(str, req.length()), boost::bind(&Network::handleWrite, this, boost::asio::placeholders::error));
  boost::asio::async_read(_sock, boost::asio::buffer(_buff, max_length), boost::bind(&Network::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

std::string Network::getAnswer() {
  if (_answers.empty())
    return "";
  std::string tmp = _answers.back();
  _answers.pop_back();
  return tmp;
}