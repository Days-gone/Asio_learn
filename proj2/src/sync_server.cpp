#include <set>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#define LOG(msg) printf("[LOG]%s:%d\n\t%s\n", __FILE__, __LINE__, msg);
using namespace boost::asio::ip;
using socket_ptr = std::shared_ptr<tcp::socket>;
constexpr int MAX_LENGTH = 1024;
std::set<std::shared_ptr<std::thread>> thread_set;

void session(socket_ptr sock)
{
    // If reads and writes are processed in the main thread, reads and writes
    // from other threads are blocked, so a new thread is created to handle them
    try
    {
        while (1)
        {
            char data[MAX_LENGTH];
            memset(data, 0, MAX_LENGTH);
            boost::system::error_code ec;
            // std::size_t length = boost::asio::read(sock,
            // boost::asio::buffer(data, MAX_LENGTH), ec);
            std::size_t length =
                sock->read_some(boost::asio::buffer(data, MAX_LENGTH), ec);
            if (ec == boost::asio::error::eof)
            {
                std::cout << "connection closed by peer" << std::endl;
            }
            else if (ec)
            {
                throw boost::system::system_error(ec);
            }

            std::cout << "receive from : "
                      << sock->remote_endpoint().address().to_string() << '\n';
            std::cout << "receive message: " << data << '\n';
            // return the message to client
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << " <- Error\n";
    }
}

void server(boost::asio::io_context& ioc, unsigned port)
{
    tcp::acceptor a(ioc, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr socket(new tcp::socket(ioc));
        a.accept(*socket);
        auto t = std::make_shared<std::thread>(session, socket);
        // use thread reference level to make sure that the thread we made
        // will not be recycle after one turn in the for loop
        thread_set.insert(t);
    }
}

int main()
{
    try
    {
        boost::asio::io_context ioc;
        server(ioc, 10086);
        for (auto& t : thread_set)
        {
            t->join();
        }
    }
    catch (std::exception& e)
    {
        LOG(e.what());
    }
}