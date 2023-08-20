#ifndef CLI_H
#define CLI_H
#include <boost/asio.hpp>
#include <cstdlib>
#include <data.hpp>
#include <iostream>
#include <string>

class ChatClient
{
public:
    ChatClient(const std::string &serverIP, unsigned short serverPort,
               unsigned short client_Port)
        : m_serverIP(serverIP),
          m_serverPort(serverPort),
          client_Port(client_Port),
          m_socket(m_ioService)
    {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), client_Port);
        m_socket.open(endpoint.protocol());
        m_socket.bind(endpoint);
    }

    // 连接到服务器
    void connect();

    // 发送消息
    void sendMessage(const std::string &message);

    void show_self_ip();

private:
    std::string processMessage(const std::string &message);

    void waitForMessage();

private:
    std::string m_serverIP;
    unsigned short m_serverPort;
    unsigned short client_Port;
    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
};

#endif