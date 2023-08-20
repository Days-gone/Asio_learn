#include <cli.hpp>

using namespace boost::asio;
using ip::tcp;

// 连接到服务器
void ChatClient::connect()
{
    tcp::resolver resolver(m_ioService);
    tcp::resolver::query query(m_serverIP, std::to_string(m_serverPort));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        m_socket.close();
        m_socket.connect(*endpoint_iterator++, error);
    }

    if (error)
    {
        throw boost::system::system_error(error);
    }

    // 启动异步接收消息
    // waitForMessage();
    // m_ioService.run();
}

// 发送消息
void ChatClient::sendMessage(const std::string &message)
{
    // preprocess the message for the blank in the string
    std::string data = message + "\n";
    boost::asio::write(m_socket, boost::asio::buffer(data));
}

void ChatClient::show_self_ip()
{
    std::cout << m_socket.local_endpoint().address().to_string() << "\n"
              << m_socket.local_endpoint().port() << "\n";
}

std::string ChatClient::processMessage(const std::string &message)
{
    std::string processedMessage = message;
    std::size_t pos = 0;
    while ((pos = processedMessage.find(' ', pos)) != std::string::npos)
    {
        processedMessage.replace(pos, 1, "_"); // 用特殊字符 "_" 代替空格
        pos += 1;
    }
    return processedMessage;
}

void ChatClient::waitForMessage()
{
    boost::asio::async_read_until(
        m_socket, m_buffer, '\n',
        [this](const boost::system::error_code &error,
               std::size_t bytes_transferred) {
            if (!error)
            {
                std::string message(
                    boost::asio::buffers_begin(m_buffer.data()),
                    boost::asio::buffers_begin(m_buffer.data()) +
                        bytes_transferred);

                // 处理接收到的消息，这里可以调用回调函数等
                std::cout << "Received message: " << message << std::endl;

                // 继续异步接收消息
                waitForMessage();
            }
        });
}
