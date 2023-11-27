
#include "sniper_sdk.h"

std::mutex sessionIDMutex;
std::map<uint32_t, asio::ip::tcp::socket> session_map;
uint32_t sessionId = 0;

// 全局变量，存储连接后的socket
//std::shared_ptr<asio::ip::tcp::socket> global_socket = nullptr;

std::vector<uint8_t> head_buffer(sn_totalHeadLength);
std::vector<uint8_t> temp_buffer(temp_length);
std::vector<uint8_t> total_buffer;
uint64_t bodylength = 0; // 代表数据长度

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
        std::filesystem::path rootdir = filehelper::rootDir;
        std::cout << "根目录为:" + rootdir.string() << std::endl;
        filehelper::DecompressFolder(total_buffer, rootdir.string());
        std::cout << "解压完成" << std::endl;
        return; // 将这句代码去掉，则会一直运行io_context.run
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
    if (bytes_transferred >= sn_totalHeadLength) // 首先将头取出来
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
      std::vector<uint8_t> BodyData(buffer.begin() + sn_headLength, buffer.begin() + sn_headLength + sn_bodyLength);
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
    // get_sessionid();
    // session_map.insert(std::make_pair(sessionId,std::move(socket)));

    // 存储socket到全局变量
    // global_socket = std::make_shared<asio::ip::tcp::socket>(std::move(socket));

    asio::async_read(socket, asio::buffer(head_buffer),
                     std::bind(read_head, std::placeholders::_1, std::placeholders::_2, std::ref(head_buffer), std::ref(socket)));

    asio::write(socket, asio::buffer(GET_ALL_FILE));
    std::cout << "Send message: " << GET_ALL_FILE << std::endl;
  }
  else
  {
    std::cerr << "Connect error: " << error.message() << std::endl;
  }
}

uint32_t connect_to_server(std::string server_host, std::string port)
{
  filehelper::rootDir = util::get_executablePath();
  std::cout << "current PATH:" + filehelper::rootDir.string() << std::endl;
  try
  {
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server_host, port);
    asio::async_connect(socket, endpoints, std::bind(connect_handler, std::placeholders::_1, std::placeholders::_2, std::ref(socket)));
    io_context.run();
  }
  catch (std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  return 1;
}

void send_message(uint32_t sessionId, std::string message)
{
  //asio::write(global_socket, asio::buffer(message));
  std::cout << "Send message: " << message << std::endl;
  /* auto it = session_map.find(sessionId);
    // 检查是否找到键
    if (it != session_map.end())
    {
        asio::ip::tcp::socket& m_socket = it->second;

    }
    else
    {
        std::cout << sessionId << " not found." << std::endl;
    }*/
}

uint32_t get_sessionid()
{
  std::lock_guard<std::mutex> lock(sessionIDMutex); // 锁定互斥锁
  // 生成唯一的 session ID
  sessionId = sessionId + 1;
  return sessionId;
}