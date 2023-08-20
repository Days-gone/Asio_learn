#include <endpoint.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
using namespace boost;

int client_end_point()
{
    std::string raw_ip_addr = "127.0.0.1";
    unsigned short port = 3333;
    // boost system error_code is for telling you which error is made
    boost::system::error_code ec;
    asio::ip::address ip_addr = asio::ip::address::from_string(raw_ip_addr, ec);
    if (ec.value() != 0)
    {
        LOG(ec.message().c_str());
        return ec.value();
    }

    // make a endpoint
    // endpoint = ipaddr + port
    asio::ip::tcp::endpoint ep(ip_addr, port);
    return 0;
}

int server_end_point()
{
    unsigned short port = 3333;
    // server give a ip, any for anyone can connect with me
    asio::ip::address server_ip = asio::ip::address_v4::any();
    asio::ip::tcp::endpoint ep(server_ip, port);
    return 0;
}

int create_tcp_socket()
{
    asio::io_context ioc;
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    asio::ip::tcp::socket sock(ioc);
    boost::system::error_code ec;
    sock.open(protocol, ec);
    if (ec.value() != 0)
    {
        LOG("Can't open ")
        return ec.value();
    }
    return 0;
}

int create_accptor_socket()
{
    asio::io_context ios;
    // use accptor to accpt a connection from client
    // use port and ip and io_context to generate a acceptor
    // old version is shown in the bind_acceptor_socket func

    //---new version
    asio::ip::tcp::acceptor a(
        ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3333));
    return 0;
}

int bind_acceptor_socket()
{
    //---old version
    unsigned short p = 3333;
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), p);
    asio::io_context ios;
    asio::ip::tcp::acceptor ac(ios, ep.protocol());
    boost::system::error_code ec;
    ac.bind(ep, ec);
    if (ec.value() != 0)
    {
        LOG("failed here");
        return ec.value();
    }
    return 0;
}

int connect_to_end()
{
    // client connect to server
    std::string raw_ip_addr = "192.168.1.124";
    unsigned short p = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_addr),
                                   p);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
    }
    catch (system::system_error &e)
    {
        LOG("error!");
        return e.code().value();
    }
    return 0;
}

int dns_connect()
{
    std::string host = "daysgone";
    std::string portnum = "3333";
    asio::io_context ios;
    // make a dns-parser query
    asio::ip::tcp::resolver::query rq(
        host, portnum, asio::ip::tcp::resolver::query::numeric_service);
    // make a dns-parser
    asio::ip::tcp::resolver resolver(ios);
    try
    {
        // resolver's member func: resolve return a iterator of ip-list under
        // the given dns
        asio::ip::tcp::resolver::iterator it = resolver.resolve(rq);
        asio::ip::tcp::socket sock(ios);
        // sock.connect() need a ip ,change to use another func in asio lib
        asio::connect(sock, it);
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
        return e.code().value();
    }
    return 0;
}

int accept_connect()
{
    constexpr int BLOCK_SIZE = 30;
    unsigned short port = 3333;
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);
    asio::io_context ios;
    try
    {
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(BLOCK_SIZE);
        // a new sock to communicate with client
        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
        return e.code().value();
    }
    return 0;
}

void self_const_bufffer()
{
    std::string text = "hello world\n";
    asio::const_buffer asio_buf(text.c_str(), text.length());
    std::vector<asio::const_buffer> buffer_sequence;
    buffer_sequence.push_back(asio_buf);
}

void simple_buffer()
{
    asio::const_buffers_1 output = asio::buffer("hello world");
}

void write_socket(asio::ip::tcp::socket &sock)
{
    std::string buf = "hello";
    std::size_t total_bytes_writen = 0;
    while (total_bytes_writen != buf.length())
    {
        total_bytes_writen +=
            sock.write_some(asio::buffer(buf.c_str() + total_bytes_writen,
                                         buf.length() - total_bytes_writen));
    }
}

void write_by_write_some()
{
    // for the tcp kernel buffer size is limited, your text may only write some
    // of them and write_some return the practical bytes it has sent use send to
    // streamline the process
    std::string raw_ip_addr = "123.0.0.1";
    unsigned short port = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_addr),
                                   port);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        write_socket(sock);
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
    }
}

void write_by_send()
{
    // target: send my message and don't remind me
    // until tcp has send all of my message
    // in sync: how to reach the target:
    std::string raw_ip_addr = "123.0.0.1";
    unsigned short port = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_addr),
                                   port);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        std::string buf = "hello";
        int send_status = sock.send(asio::buffer(buf.c_str(), buf.length()));
        // send_status only will be : <0 =0 >0
        // if <0 : system error
        // if =0 : connection close
        // if >0 : return the length of buf, for send is blocking until the
        // message finish
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
    }
}

std::string read_from_socket(boost::asio::ip::tcp::socket &s)
{
    constexpr unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;
    while (total_bytes_read != MESSAGE_SIZE)
    {
        total_bytes_read += s.read_some(asio::buffer(
            buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
    }
    return std::string(buf, total_bytes_read);
}

int read_data_by_read_some()
{
    std::string ip = "192.168.1.1";
    unsigned short p = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), p);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        read_from_socket(sock);
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
        return e.code().value();
    }
    return 0;
}

int read_data_by_receive()
{
    std::string ip = "192.168.1.1";
    unsigned short p = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), p);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        const unsigned char BUFF_SIZE = 7;
        char buffer_receive[BUFF_SIZE];
        // same as the write data by send
        int receive_status =
            sock.receive(asio::buffer(buffer_receive, BUFF_SIZE));
        // you can use global func read as well
        // int receive_status =
        // asio::read(sock,asio::buffer(buffer_receive,BUFF_SIZE));
    }
    catch (system::system_error &e)
    {
        LOG(e.what());
        return e.code().value();
    }
    return 0;
}

int main() { std::cout << "hello\n"; }