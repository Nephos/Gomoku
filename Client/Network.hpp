#pragma once

#include <utility>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#define max_length 256

class Network
{
  public:
    Network(std::string const &, std::string const &);
    ~Network() {};

    void init();
    void connect();
    void update();
    void sendQuery(const std::string *);
    bool isConnected();

    std::string getAnswer();

    void handleConnect(const boost::system::error_code&);

  private:
    boost::asio::io_service _io_service;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _sock;
    boost::asio::ip::tcp::resolver::iterator _it;
    char _buff[max_length];
    std::vector<std::string> _answers;
    std::string const &_host;
    std::string const &_port;
};