#pragma once
#include <map>
#include <boost/asio.hpp>
#include "Session.hpp"


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