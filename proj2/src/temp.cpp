#include "async_server.hpp"

int main()
{
    try
    {
        boost::asio::io_context ioc;
        Server s(ioc, 8080);
        ioc.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
}