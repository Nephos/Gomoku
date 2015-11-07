#pragma once

#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#define max_length 256

class Network
{
  public:
    Network();
    ~Network() {};

    void init();
    void connect(std::string const &, std::string const &);
    void update();
    void sendMove(const std::pair<int, int> *);
    void sendQuery(const std::string *);
    bool isConnected();

    void handleConnect(const boost::system::error_code&);
    void handleWrite(const boost::system::error_code&);
    void handleRead(const boost::system::error_code&);
    void timerHandle(const boost::system::error_code &);

    boost::asio::io_service _io_service;
  private:
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _sock;
    boost::asio::ip::tcp::resolver::iterator _it;
    char _buff[max_length];
    // Vector of replies to treat
    bool _connected;
};