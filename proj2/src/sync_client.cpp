#include <cstddef>
#include <iostream>
#include <boost/asio.hpp>
#define LOG(msg) printf("[LOG]%s:%d\n\t%s\n", __FILE__, __LINE__, msg);
using namespace boost::asio::ip;
constexpr int MAX_LENGTH = 1024;

int main()
{
    try
    {
        boost::asio::io_context ioc;
        tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 8080);
        tcp::socket sock(ioc);
        boost::system::error_code ec = boost::asio::error::host_not_found;
        sock.connect(remote_ep, ec);
        if (ec.value() != 0)
        {
            LOG(ec.message().c_str());
            return ec.value();
        }
        std::cout << "Give Your Message:\n";
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        std::size_t req_length = strlen(request);
        boost::asio::write(sock, boost::asio::buffer(request));

        // receive as much as i have sent!
        char reply[MAX_LENGTH];
        std::size_t reply_length =
            boost::asio::read(sock, boost::asio::buffer(reply, req_length));
        std::cout << "Reply is:";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch (std::exception& e)
    {
        LOG(e.what());
        return -1;
    }
}