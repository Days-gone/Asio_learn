#include "async_server.hpp"

using namespace boost::asio::ip;

Session::Session(boost::asio::io_context& ioc, Server* server)
    : _socket(ioc), _server(server)
{
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(a_uuid);
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
        std::cerr << "Read Error: " << ec.message() << "\n";
        _server->ClearSession(_uuid);
    }
    else
    {
        std::cout << "receive data is " << _data << std::endl;
        boost::asio::async_write(
            _socket, boost::asio::buffer(_data, max_length),
            std::bind(&Session::HandleWrite, this, std::placeholders::_1,
                      _self_shared));
    }
}

void Session::HandleWrite(const boost::system::error_code& ec,
                           std::shared_ptr<Session> _self_shared)
{
    if (ec.value() != 0)
    {
        std::cerr << "Write Error" << ec.message() << "\n";
        _server->ClearSession(_uuid);
    }
    else
    {
        memset(_data, 0, max_length);
        boost::asio::async_read(
            _socket, boost::asio::buffer(_data, max_length),
            std::bind(&Session::HandleRead, this, std::placeholders::_1,
                      std::placeholders::_2, _self_shared));
    }
}

Server::Server(boost::asio::io_context& ioc, unsigned short port)
    : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "server start successfully" << std::endl;
    StartAccept();
}

void Server::StartAccept()
{
    std::shared_ptr<Session> new_session =
        std::make_shared<Session>(_ioc, this);
    // Session* new_session = new Session(_ioc);
    _acceptor.async_accept(new_session->Socket(),
                           std::bind(&Server::HandleAccept, this, new_session,
                                     std::placeholders::_1));
}

void Server::HandleAccept(std::shared_ptr<Session> new_session,
                           const boost::system::error_code& ec)
{
    if (ec.value() != 0)
    {
        std::cerr << "Fail" << ec.message() << std::endl;
        delete this;
    }
    else
    {
        new_session->Start();
        _sessions.insert(std::make_pair(new_session->Getuuid(), new_session));
    }
    StartAccept();
}

void Server::ClearSession(std::string uuid){
    _sessions.erase(uuid);
}