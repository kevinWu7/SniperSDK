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