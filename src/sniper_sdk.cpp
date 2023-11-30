
#include "sniper_sdk.h"

std::mutex sessionIDMutex;
std::map<uint32_t, asio::ip::tcp::socket> session_map;
uint32_t sessionId = 0;

// 在全局范围内声明全局 socket 和 io_context 对象
asio::io_context main_io_context;
asio::ip::tcp::socket main_socket(main_io_context);
// 是否成功建立连接
bool isConnected = false;

std::vector<uint8_t> head_buffer(sn_totalHeadLength);
std::vector<uint8_t> temp_buffer(temp_length);
std::vector<uint8_t> total_buffer;
uint64_t bodylength = 0;                 // 代表数据长度
std::function<void()> connected_success; // 连接成功回调

void connect_to_server_async(std::string server_host, std::string port, std::function<void()> _connected_success)
{
  connected_success = _connected_success;
  std::thread myThread(connect_to_server, server_host, port);
  // 将该线程和connect_to_server_async方法解除关系。防止方法结束直接释放myThread对象导致报错。
  // myThread.detach();
  // 等待myThread结束。加入此句会阻塞当前的进程
  myThread.join();
}

void connect_to_server(std::string server_host, std::string port)
{
  filehelper::rootDir = util::get_executablePath();
  std::cout << "current PATH:" + filehelper::rootDir.string() << std::endl;
  try
  {
    asio::ip::tcp::resolver resolver(main_io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server_host, port);
    asio::async_connect(main_socket, endpoints, std::bind(connect_handler, std::placeholders::_1, std::placeholders::_2, std::ref(main_socket)));
    // asio的异步模型，此句会一直等待直到完成所有事情。流程如下：
    // 1、由main_socket通过async_connect绑定了connect_handler
    // 2、connect_handler内部，又通过async_read绑定了read_head
    // 3、read_head内部，通过async_read_some，绑定了read_data
    // 4、read_data内部分两种情况，第一种根据读取数据解压完成，并直接return，此时main_io_context.run();被释放，不再阻塞
    // 第二种，继续绑定自身，一直到读取到符合预期的数据为止
    main_io_context.run();
  }
  catch (std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}

void connect_handler(const asio::error_code &error, const asio::ip::tcp::endpoint &endpoint, asio::ip::tcp::socket &socket)
{
  if (!error)
  {
    std::cout << "success connected to server: " << endpoint << std::endl;

    asio::async_read(socket, asio::buffer(head_buffer),
                     std::bind(read_head, std::placeholders::_1, std::placeholders::_2, std::ref(head_buffer), std::ref(socket)));
    isConnected = true;
    connected_success();
  }
  else
  {
    std::cerr << "Connect error: " << error.message() << std::endl;
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

void send_message(std::string message)
{
  if (isConnected)
  {
    asio::write(main_socket, asio::buffer(message));
    std::cout << "Send message: " << message << std::endl;
  }
}

uint32_t get_sessionid()
{
  std::lock_guard<std::mutex> lock(sessionIDMutex); // 锁定互斥锁
  // 生成唯一的 session ID
  sessionId = sessionId + 1;
  return sessionId;
}