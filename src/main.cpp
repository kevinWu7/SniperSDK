#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "asio.hpp"
#include "zlib.h"
#include "baseinfo.h"
#include "filehelper.h"

void read_handler(const asio::error_code &error,
                  std::size_t bytes_transferred,
                  std::array<char, 1024> &buffer,
                  asio::ip::tcp::socket &socket)
{
  if (!error)
  {
    // Process the received data
    std::string response(buffer.data(), bytes_transferred);
    std::cout << "Server response: " << response << std::endl;

    // Continue reading for more data
    socket.async_read_some(asio::buffer(buffer),
                           std::bind(read_handler, std::placeholders::_1, std::placeholders::_2,
                                     std::ref(buffer), std::ref(socket)));
  }
  else if (error == asio::error::eof)
  {
    std::cerr << "Server closed the connection" << std::endl;
  }
  else
  {
    std::cerr << "Read error: " << error.message() << std::endl;
  }
}

void connect_handler(const asio::error_code &error, const asio::ip::tcp::endpoint &endpoint, asio::ip::tcp::socket &socket)
{
  if (!error)
  {
    std::cout << "Connected to server: " << endpoint << std::endl;

    // Send a message to the server
    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    // Create the desired format
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y%m%d %H:%M:%S");
    std::string formattedTime = oss.str();
    std::string message = "GET_ALL_FILE\n";
    asio::write(socket, asio::buffer(message));
    std::cout << "Send message: " << message << std::endl;

    /*  std::array<char, 1024> buffer;
asio::async_read(socket, asio::buffer(buffer),
  std::bind(read_handler, std::placeholders::_1, std::placeholders::_2,
            std::ref(buffer), std::ref(socket)));*/
    // 使用同步read_some后，就能正常收到消息了

    std::vector<uint8_t> buffer;
    const std::size_t chunkSize = 1024;

    while (true)
    {
      char tempBuffer[chunkSize];
      std::size_t bytesRead = socket.read_some(asio::buffer(tempBuffer));
      std::cout << "bytesRead= : " << std::to_string(bytesRead) << std::endl;

      buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
      std::cout << "read buffer's size : " << std::to_string(buffer.size()) << std::endl;
      if (bytesRead < chunkSize || bytesRead == 0)
      {
        break; // 读取完成
      }
    }
    std::cout << "receive message length: " << std::to_string(buffer.size()) << std::endl;
    std::filesystem::path targetDir2 = filehelper::rootDir;
    filehelper::DecompressFolder(buffer, targetDir2.append("realdata"));
  }
  else
  {
    std::cerr << "Connect error: " << error.message() << std::endl;
  }
}

void start_client()
{
  try
  {
    // Create an io_context object
    asio::io_context io_context;
    // Create a TCP socket object
    asio::ip::tcp::socket socket(io_context);

    // Resolve the server address and port
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server_host, "18889");

    // Connect to the server
    asio::async_connect(socket, endpoints, std::bind(connect_handler, std::placeholders::_1, std::placeholders::_2, std::ref(socket)));

    // Run the io_context to start processing the asynchronous operations
    io_context.run();
    std::cout << "IO context finished." << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}

int main(int argc, char *argv[])
{
  if (argc < 1)
  {
    std::cerr << "No command-line arguments provided." << std::endl;
    return 1;
  }
  filehelper::rootDir = filehelper::getBaseDir(argv[0]);

  std::cout << "argv:" + std::string(argv[0]) << std::endl;
  std::cout << "current Path:" + filehelper::rootDir.string() << std::endl;

  start_client();
  std::cout << "Reached the end" << std::endl;
  return 0;
}
