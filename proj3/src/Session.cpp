#include "Session.hpp"
#include "Server.hpp"

using namespace boost::asio::ip;

Session::Session(boost::asio::io_context& ioc, Server* server)
    : _socket(ioc), _server(server)
{
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(a_uuid);
}

void Session::Send(char* msg, int maxlength)
{
    bool pending = false;
    std::lock_guard<std::mutex> lock(_send_lock);
    if (_send_queue.size() > 0)
    {
        pending = true;
    }

    _send_queue.push(std::make_shared<MsgNode>(msg, maxlength));

    if (pending)
    {
        return;
    }

    boost::asio::async_write(
        _socket, boost::asio::buffer(msg, maxlength),
        std::bind(&Session::HandleWrite, this, std::placeholders::_1,
                  shared_from_this()));
}

tcp::socket& Session::Socket() { return _socket; }

void Session::Start()
{
    memset(_data, 0, max_length);
    _socket.async_read_some(
        boost::asio::buffer(_data, max_length),
        std::bind(&Session::HandleRead, this, std::placeholders::_1,
                  std::placeholders::_2, shared_from_this()));
}

std::string& Session::Getuuid() { return this->_uuid; }

void Session::HandleRead(const boost::system::error_code& ec,
                         std::size_t bytes_transferred,
                         std::shared_ptr<Session> _self_shared)
{
    // if i received message then echo back to the sender
    if (ec.value() != 0)
    {
        std::cerr << "[ERROR] Read Error: " << ec.message() << "\n";
        _server->ClearSession(_uuid);
    }
    else
    {
        std::cout << "receive data is " << _data << std::endl;
        Send(_data,bytes_transferred);
        memset(_data, 0, max_length);
        _socket.async_read_some(
            boost::asio::buffer(_data, max_length),
            std::bind(&Session::HandleRead, this, std::placeholders::_1,
                      std::placeholders::_2, _self_shared));
    }
}

void Session::HandleWrite(const boost::system::error_code& ec,
                          std::shared_ptr<Session> _self_shared)
{
    if (ec.value() != 0)
    {
        std::cerr << "[ERROR] Write Error: " << ec.message() << "\n";
        _server->ClearSession(_uuid);
    }
    else
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        _send_queue.pop();
        if (!_send_queue.empty())
        {
            auto& msg = _send_queue.front();
            boost::asio::async_write(
                _socket, boost::asio::buffer(msg->_msg, msg->_total_len),
                std::bind(&Session::HandleWrite, this, std::placeholders::_1,
                          shared_from_this()));
        }
    }
}