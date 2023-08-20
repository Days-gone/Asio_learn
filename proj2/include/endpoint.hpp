#pragma once
#include <iostream>
#include <boost/asio.hpp>
// #include <boost/asio/buffer.hpp>
// #include <boost/asio/connect.hpp>
// #include <boost/asio/io_context.hpp>
// #include <boost/asio/ip/address.hpp>
// #include <boost/asio/ip/address_v4.hpp>
// #include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/buffer.hpp>
#include <vector>
#define LOG(msg) printf("[LOG]%s:%d\n\t%s\n", __FILE__, __LINE__, msg);

extern int client_end_point();
extern int server_end_point();
extern int create_tcp_socket();
extern int create_accptor_socket();
extern int bind_acceptor_socket();
extern int connect_to_end();
extern int dns_connect();
extern int accept_connect();
extern void self_const_bufffer();
extern void simple_buffer();
extern void write_socket(boost::asio::ip::tcp::socket& s);
extern void write_by_write_some();
extern void write_by_send();
extern std::string read_from_socket(boost::asio::ip::tcp::socket& s);
extern int read_data_by_read_some();
extern int read_data_by_receive();
