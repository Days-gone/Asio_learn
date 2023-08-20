#include "session.hpp"
//a introduction demo to the asio async IO api
using namespace boost;

Session::Session(std::shared_ptr<asio::ip::tcp::socket> socket)
    : _socket(socket), _send_pending(false)
{
}

void Session::Connect(const boost::asio::ip::tcp::endpoint &ep) {}

void Session::WriteCallBackErr(const boost::system::error_code &ec,
                               std::size_t bytes_transferred,
                               std::shared_ptr<MsgNode> msg_node)
{
    if (bytes_transferred + msg_node->_cur_len < msg_node->_total_len)
    {
        _send_node->_cur_len += bytes_transferred;
        this->_socket->async_write_some(
            asio::buffer(_send_node->_msg + _send_node->_cur_len,
                         _send_node->_total_len - _send_node->_cur_len),
            std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1,
                      std::placeholders::_2, _send_node));
    }
}

void Session::WriteToSocketErr(const std::string buf)
{
    _send_node = std::make_shared<MsgNode>(buf.c_str(), buf.length());
    this->_socket->async_write_some(
        asio::buffer(_send_node->_msg, _send_node->_total_len),
        // give a class member func should give the member pointer also !
        // so comes the first and the second parameters
        std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1,
                  std::placeholders::_2, _send_node));
}

void Session::WriteToSocket(const std::string buf)
{
    _send_queue.emplace(std::make_shared<MsgNode>(buf.c_str(), buf.length()));
    if (_send_pending)
    {
        return;
    }
    this->_socket->async_write_some(
        asio::buffer(buf),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
    this->_send_pending = true;
}

void Session::WriteCallBack(const boost::system::error_code &ec,
                            std::size_t bytes_transferred)
{
    if (ec.value() != 0)
    {
        std::cerr << "erorr!" << ec.message() << "\n";
        return;
    }
    auto &send_data = _send_queue.front();
    send_data->_cur_len += bytes_transferred;
    if (send_data->_cur_len < send_data->_total_len)
    {
        this->_socket->async_write_some(
            asio::buffer(send_data->_msg + send_data->_cur_len,
                         send_data->_total_len - send_data->_cur_len),
            std::bind(&Session::WriteCallBack, this, std::placeholders::_1,
                      std::placeholders::_2));
        return;
    }

    _send_queue.pop();

    if (_send_queue.empty())
    {
        _send_pending = false;
    }
    else
    {
        auto &send_data = _send_queue.front();
        this->_socket->async_write_some(
            asio::buffer(send_data->_msg + send_data->_cur_len,
                         send_data->_total_len - send_data->_cur_len),
            std::bind(&Session::WriteCallBack, this, std::placeholders::_1,
                      std::placeholders::_2));
    }
}

void Session::WriteAllToSocket(const std::string buf)
{
    _send_queue.emplace(std::make_shared<MsgNode>(buf.c_str(), buf.length()));
    if (_send_pending)
    {
        return;
    }
    this->_socket->async_send(
        asio::buffer(buf),
        std::bind(&Session::WriteAllCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
}

void Session::WriteAllCallBack(boost::system::error_code &ec, std::size_t)
{
    _send_queue.pop();
    if (_send_queue.empty())
    {
        _send_pending = false;
        return;
    }
    else
    {
        // queeu is not empty send the next MsgNode for the first time
        auto &send_data = _send_queue.front();
        this->_socket->async_send(
            asio::buffer(send_data->_msg, send_data->_total_len),
            std::bind(&Session::WriteAllCallBack, this, std::placeholders::_1,
                      std::placeholders::_2));
    }
}

void Session::ReadFromSocket()
{
    if (_recv_pending) return;

    _recv_node = std::make_shared<MsgNode>(RECVSIZE);
    _socket->async_read_some(
        asio::buffer(_recv_node->_msg, _recv_node->_total_len),
        std::bind(&Session::ReadCallBack, this, std::placeholders::_1,
                  std::placeholders::_2));
    _recv_pending = true;
}

void Session::ReadCallBack(boost::system::error_code &ec,
                           std::size_t bytes_transferred)
{
    _recv_node->_cur_len += bytes_transferred;
    if (_recv_node->_cur_len < _recv_node->_total_len)
    {
        _socket->async_read_some(
            asio::buffer(_recv_node->_msg + _recv_node->_cur_len,
                         _recv_node->_total_len - _recv_node->_cur_len),
            std::bind(&Session::ReadCallBack, this, std::placeholders::_1,
                      std::placeholders::_2));
        return;
    }
    _recv_pending = true;
    _recv_node = nullptr;
}

void Session::ReadAllFromSocket()
{
    if (_recv_pending)
    {
        return;
    }
    else
    {
        _recv_node = std::make_shared<MsgNode>(RECVSIZE);
        _socket->async_receive(
            asio::buffer(_recv_node->_msg, _recv_node->_total_len),
            std::bind(&Session::WriteAllCallBack, this, std::placeholders::_1,
                      std::placeholders::_2));
        _recv_pending = true;
    }
}

void Session::ReadAllCallBack(boost::system::error_code &ec,
                              std::size_t bytes_transferred)
{
    _recv_node->_cur_len += bytes_transferred;
    _recv_node = nullptr;
    _recv_pending = false;
}