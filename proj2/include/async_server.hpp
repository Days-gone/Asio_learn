#pragma once
#include <map>
#include <cstddef>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_context& ioc, Server* server);

    boost::asio::ip::tcp::socket& Socket();

    void Start();

    std::string& Getuuid();

private:
    void HandleRead(const boost::system::error_code& ec,
                     std::size_t bytes_transferred,
                     std::shared_ptr<Session> _self_shared);
    void HandleWrite(const boost::system::error_code& ec,
                      std::shared_ptr<Session> _self_shared);

private:
    boost::asio::ip::tcp::socket _socket;
    Server* _server;
    enum
    {
        max_length = 1024
    };
    char _data[max_length];
    std::string _uuid;
};

class Server
{
public:
    explicit Server(boost::asio::io_context& ioc, unsigned short port);
    void ClearSession(std::string uuid);

private:
    void StartAccept();
    void HandleAccept(std::shared_ptr<Session> new_session,
                       const boost::system::error_code& ec);
    boost::asio::io_context& _ioc;
    boost::asio::ip::tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<Session>> _sessions;
};