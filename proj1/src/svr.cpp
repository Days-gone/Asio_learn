#include "svr.hpp"
#include "data.hpp"

// 启动服务器
void ChatServer::start()
{
    doAccept();
    m_ioService.run();
}

// 发送消息给指定的IP地址
void ChatServer::sendMessage(const std::string &receiverIP,
                             const std::string &port,
                             const std::string &message)
{
    tcp::resolver resolver(m_ioService);
    tcp::resolver::query query(receiverIP, port);
    tcp::resolver::iterator endpointIterator = resolver.resolve(query);

    boost::asio::async_connect(
        m_socket, endpointIterator,
        [this, message](const boost::system::error_code &error,
                        const tcp::resolver::iterator &iterator) {
            if (!error)
            {
                boost::asio::async_write(
                    m_socket, boost::asio::buffer(message + "\n"),
                    [this](const boost::system::error_code &, std::size_t) {});
            }
        });
}

void ChatServer::doAccept()
{
    m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec) {
        if (!ec)
        {
            // 连接成功，启动异步接收消息
            doReceive();
        }

        // 继续等待新的连接
        doAccept();
    });
}

void ChatServer::doReceive()
{
    boost::asio::async_read_until(
        m_socket, m_buffer, '\n',
        [this](const boost::system::error_code &error,
               std::size_t bytes_transferred) {
            if (!error)
            {
                // use a string iterator range to generate a new string
                // message
                std::string message(
                    boost::asio::buffers_begin(m_buffer.data()),
                    boost::asio::buffers_begin(m_buffer.data()) +
                        bytes_transferred);

                // 处理接收到的消息，这里可以调用回调函数等
                handleMessage(message);

                // 清除缓冲区中的数据
                m_buffer.consume(bytes_transferred);

                // 继续异步接收消息
                doReceive();
            }
        });
}

void ChatServer::handleMessage(const std::string &message)
{
    DataParser received_parser(message);
    std::cout << "message is : " << message;
    std::cout << "Received message:\n" + received_parser.extractValue("content")
              << "\n";
    std::string forward_IP = received_parser.extractValue("receiverIP");
    std::string forward_Port = received_parser.extractValue("receiverPort");
    std::string content = received_parser.extractValue("content");
    // sendMessage(forward_IP, forward_Port, content);
}