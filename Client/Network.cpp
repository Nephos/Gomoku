#include "Exceptions.hpp"
#include "Network.hpp"

Network::Network() : _resolver(_io_service), _sock(_io_service) {}

void Network::connect(std::string const &host, std::string const &port) {
  _connected = false;
  boost::asio::ip::tcp::resolver::query query(host, port);
  boost::asio::ip::tcp::resolver::iterator iterator = _resolver.resolve(query);
  boost::asio::async_connect(_sock.lowest_layer(), iterator, boost::bind(&Network::handleConnect, this, boost::asio::placeholders::error));
}

void Network::handleConnect(const boost::system::error_code &err) {
  if (err)
    throw Gomoku::NetworkException(err.message());
  _connected = true;
  std::cout << "Connected" << std::endl;
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
  _io_service.poll();
}

void Network::sendMove(const std::pair<int, int> *move) {
  boost::asio::async_write(_sock, boost::asio::buffer(move, sizeof(move)), boost::bind(&Network::handleWrite, this, boost::asio::placeholders::error));
}

void Network::sendQuery(const std::string *req) {
  const char *str = new char[req->length()];
  str = req->c_str();
  boost::asio::async_write(_sock, boost::asio::buffer(str, req->length()), boost::bind(&Network::handleWrite, this, boost::asio::placeholders::error));
  _io_service.run();
}