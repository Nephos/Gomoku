#include "Exceptions.hpp"
#include "Network.hpp"

Network::Network() : _resolver(_io_service), _sock(_io_service) {}

void Network::connect(std::string const &host, std::string const &port) {
  boost::asio::ip::tcp::resolver::query query(host, port);
  _resolver.async_resolve(query, boost::bind(&Network::handleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
  _io_service.run();
}

void Network::handleResolve(const boost::system::error_code &err, boost::asio::ip::tcp::resolver::iterator it) {
  if (err)
    throw Gomoku::NetworkException(err.message());
  boost::asio::ip::tcp::endpoint en = *it;
  _sock.async_connect(en, boost::bind(&Network::handleConnect, this, boost::asio::placeholders::error, ++it));
}

void Network::handleConnect(const boost::system::error_code &err, boost::asio::ip::tcp::resolver::iterator it) {
  if (err)
    throw Gomoku::NetworkException(err.message());
  if (it != boost::asio::ip::tcp::resolver::iterator()) {
    _sock.close();
    handleResolve(err, it);
  }
  else {
    _connected = true;
    std::cout << "Connected" << std::endl;
  }
}

void Network::handleWrite(const boost::system::error_code &err) {
  std::cout << "wrote something" << std::endl;
  if (err)
    throw Gomoku::NetworkException(err.message());
}

void Network::handleRead(const boost::system::error_code &err) {
  if (err)
    throw Gomoku::NetworkException(err.message());
  boost::asio::async_read(_sock, boost::asio::buffer(_buff, 256), boost::bind(&Network::handleRead, this, boost::asio::placeholders::error));
  std::cout << _buff << std::endl;
}

bool Network::isConnected() {
  return _connected;
}

void Network::update() {
  if (_connected)
    _io_service.poll();
}

void Network::sendMove(const std::pair<int, int> *move) {
  boost::asio::async_write(_sock, boost::asio::buffer(move, sizeof(move)), boost::bind(&Network::handleWrite, this, boost::asio::placeholders::error));
}

void Network::sendQuery(const std::string *req) {
  const char *str = new char[req->length()];
  str = req->c_str();
  boost::asio::async_write(_sock, boost::asio::buffer(str, req->length()), boost::bind(&Network::handleWrite, this, boost::asio::placeholders::error));
}