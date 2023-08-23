#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "asio.hpp"
#include "zlib.h"
#include "baseinfo.h"
#include "filehelper.h"
#include "util.h"

std::vector<uint8_t> head_buffer(sn_totalHeadLength);
std::vector<uint8_t> temp_buffer(temp_length);
std::vector<uint8_t> total_buffer;
uint64_t bodylength = 0;



void read_data(const asio::error_code &error, std::size_t bytes_transferred, std::vector<uint8_t> &buffer, asio::ip::tcp::socket &socket)
{
  if (!error)
  {
    if (bytes_transferred > 0)
    {
      total_buffer.insert(total_buffer.end(), buffer.begin(), buffer.begin() + bytes_transferred);
      std::cout << "total buffer size: " << std::to_string(total_buffer.size()) << std::endl;
      std::cout << "bytes_transferred: " << std::to_string(bytes_transferred) << std::endl;
      if (total_buffer.size() == bodylength)
      {
        // 数据已完全读取
        std::filesystem::path targetDir = filehelper::rootDir;
        filehelper::DecompressFolder(total_buffer, targetDir.append("realdata"));
        std::cout << "解压完成"<< std::endl;
        return;
      }
      buffer.clear();
      buffer.resize(temp_length);
      socket.async_read_some(asio::buffer(buffer),
                             std::bind(read_data, std::placeholders::_1, std::placeholders::_2,
                                       std::ref(buffer), std::ref(socket)));
    }
  }
  else
  {
    std::cerr << "Read error: " << error.message() << std::endl;
  }
}


void read_head(const asio::error_code &error, std::size_t bytes_transferred, std::vector<uint8_t> &buffer, asio::ip::tcp::socket &socket)
{
  if (!error)
  {
    if (bytes_transferred >= sn_totalHeadLength) //首先将头取出来
    {
      auto startIt = buffer.begin();                    
      auto endIt = buffer.begin() + sn_totalHeadLength; 
      std::vector<uint8_t> HeadData(startIt, endIt);
      for (int i = 0; i < sn_headLength; i++)
      {
        if (HeadData[i] != 0xFE)
        {
          std::cout << "head data error" << std::endl;
          return;
        }
      }
      std::vector<uint8_t> BodyData( buffer.begin()+sn_headLength,  buffer.begin() + sn_headLength+sn_bodyLength);
      bodylength = util::extractUint64(BodyData);
      socket.async_read_some(asio::buffer(temp_buffer),
                             std::bind(read_data, std::placeholders::_1, std::placeholders::_2,
                                       std::ref(temp_buffer), std::ref(socket)));
    }
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
    std::cout << "success connected to server: " << endpoint << std::endl;
    asio::async_read(socket, asio::buffer(head_buffer),
                     std::bind(read_head, std::placeholders::_1, std::placeholders::_2, std::ref(head_buffer), std::ref(socket)));
    std::string message = GET_ALL_FILE;
    asio::write(socket, asio::buffer(GET_ALL_FILE));
    std::cout << "Send message: " << message << std::endl;
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
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(SERVER_HOST,SERVER_PORT);
    asio::async_connect(socket, endpoints, std::bind(connect_handler, std::placeholders::_1, std::placeholders::_2, std::ref(socket)));
    io_context.run();
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
    return -1;
  }
  filehelper::rootDir = filehelper::getBaseDir(argv[0]);
  std::cout << "current Path:" + filehelper::rootDir.string() << std::endl;
  start_client();
  std::cout << "Reached the end" << std::endl;
  return 0;
}