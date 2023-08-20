#pragma once
#include <mutex>
#include <queue>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
constexpr int RECVSIZE = 1024;

class MsgNode
{
public:
    MsgNode(const char* msg, int total_len) : _total_len(total_len), _cur_len(0)
    {
        _msg = new char[RECVSIZE];
        memcpy(_msg, msg, total_len);
    }
    MsgNode(int t_len) : _total_len(t_len), _cur_len(0)
    {
        _msg = new char[t_len];
    }
    ~MsgNode() { delete[] _msg; }

    // private:
    char* _msg;
    int _total_len;
    int _cur_len;
};

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
    void Send(char* msg, int max_length);

private:
    boost::asio::ip::tcp::socket _socket;
    Server* _server;
    enum
    {
        max_length = 1024
    };
    char _data[max_length];
    std::string _uuid;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::mutex _send_lock;
};