#include "Exceptions.hpp"
#include "Network.hpp"

Network::Network(std::string const &host, std::string const &port) : _resolver(_io_service), _sock(_io_service), _host(host), _port(port) {}

void Network::connect() {
  boost::asio::ip::tcp::resolver::query query(_host, _port);
  boost::asio::ip::tcp::resolver::iterator iterator = _resolver.resolve(query);
  boost::asio::connect(_sock.lowest_layer(), iterator);
}

void Network::handleConnect(const boost::system::error_code &err) {
  if (err)
    throw Gomoku::NetworkException(err.message());
  std::cout << "Connected" << std::endl;
}

void Network::sendQuery(const std::string *req) {
  connect();
  const char *str = new char[req->length()];
  str = req->c_str();
  boost::system::error_code err;
  boost::asio::write(_sock, boost::asio::buffer(str, req->length()), err);
  if (err)
    throw Gomoku::NetworkException(err.message());
  std::cout << str << std::endl;
  size_t read = boost::asio::read(_sock, boost::asio::buffer(_buff, max_length), err);
  if (err && err.value() != 2)
    throw Gomoku::NetworkException(err.message());
  if (read > 0)
    std::cout << _buff << std::endl;
}

std::string Network::getAnswer() {
  if (_answers.empty())
    return "";
  std::string tmp = _answers.back();
  _answers.pop_back();
  return tmp;
}