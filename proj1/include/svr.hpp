#ifndef SVR_H
#define SVR_H
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <unordered_set>
#include "data.hpp"

using namespace boost::asio;
using ip::tcp;

class ChatServer
{
public:
    ChatServer(unsigned short port)
        : m_acceptor(m_ioService, tcp::endpoint(tcp::v4(), port)),
          m_socket(m_ioService),
          m_timer(m_ioService)
    {
    }
    void start();
    void sendMessage(const std::string &receiverIP, const std::string &port,
                     const std::string &message);

private:
    void doAccept();
    void doReceive();
    void handleMessage(const std::string &message);

private:
    boost::asio::io_service m_ioService;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    boost::asio::steady_timer m_timer;
};

#endif