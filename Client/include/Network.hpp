#pragma once

#include <utility>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Network
{
  public:
    Network(std::string const &, std::string const &);
    ~Network() {};

    void init();
    void connect();
    void update();
    void sendQuery(const std::string);
    std::string sendSyncQuery(const std::string &req);
    bool isConnected();
    void reset();

    std::string getAnswer();

    void handleRead(const boost::system::error_code &, size_t);
    void handleWrite(const boost::system::error_code &);

    boost::asio::io_service _io_service;

  private:
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _sock;
    boost::asio::ip::tcp::resolver::iterator _it;
    char _buff[2048];
    std::vector<std::string> _answers;
    std::string const &_host;
    std::string const &_port;
    bool _answered;
};
