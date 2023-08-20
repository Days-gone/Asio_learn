#include <svr.hpp>

int main()
{
    ChatServer server(8080);
    server.start();
    return 0;
}