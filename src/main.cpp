#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "asio.hpp"
#include "zlib.h"
#include "baseinfo.h"
#include "filehelper.h"
#include "util.h"
#include "sniper_sdk.h"


void connect_success()
{
   send_message(GET_ALL_FILE);
}

int main(int argc, char *argv[])
{
  if (argc < 1)
  {
    std::cerr << "No command-line arguments provided." << std::endl;
    return -1;
  }
  // 创建一个线程并启动
  connect_to_server_async(SERVER_HOST,SERVER_PORT,connect_success);
  std::cin.get();
  std::cout << "main function end" << std::endl;
  return 0;
}



