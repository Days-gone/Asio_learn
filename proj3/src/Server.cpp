#include "Server.hpp"

using namespace boost::asio::ip;


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