#include <iostream>
#include <asio.hpp>

using namespace asio;
using ip::tcp;

int main() {
  try {
    io_context io;

    // 创建一个 TCP 套接字
    tcp::socket socket(io);

    // 服务器的 IP 地址和端口号
    tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 18889);

    // 连接到服务器
    socket.connect(endpoint);

    // 发送 "hello world" 消息给服务器
    std::string message = "hello world\n";
    asio::write(socket, asio::buffer(message));

    // 接收并处理服务器返回的消息
    char buffer[128];
    std::size_t bytesRead = socket.read_some(asio::buffer(buffer));
    std::string response(buffer, bytesRead);
    std::cout << "服务器返回的消息: " << response << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "发生异常: " << e.what() << std::endl;
  }

  return 0;
}
