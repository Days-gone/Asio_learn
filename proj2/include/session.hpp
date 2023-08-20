#include <cstddef>
#include <memory>
#include <boost/asio.hpp>
#include <iostream>
#include <queue>
#include <functional>
#include <functional>
#include <memory>

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

class Session
{
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void Connect(const boost::asio::ip::tcp::endpoint& ep);
    void WriteCallBackErr(const boost::system::error_code& ec,
                          std::size_t bytes_transferred,
                          std::shared_ptr<MsgNode>);
    void WriteToSocketErr(const std::string buf);

    void WriteCallBack(const boost::system::error_code& ec,
                       std::size_t bytes_transferred);
    void WriteToSocket(const std::string buf);

    void WriteAllToSocket(const std::string buf);
    void WriteAllCallBack(boost::system::error_code& ec, std::size_t);

    //read
    void ReadFromSocket();
    void ReadCallBack(boost::system::error_code& ec, std::size_t);

    void ReadAllFromSocket();
    void ReadAllCallBack(boost::system::error_code& ec, std::size_t);

private:
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    std::shared_ptr<MsgNode> _send_node;
    std::shared_ptr<MsgNode> _recv_node;
    bool _send_pending;
    bool _recv_pending;
};
