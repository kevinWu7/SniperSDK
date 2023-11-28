#ifndef SNIPER_SDK_H
#define SNIPER_SDK_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <map>
#include <mutex>
#include "asio.hpp"
#include "zlib.h"
#include "baseinfo.h"
#include "filehelper.h"
#include "util.h"



//session连接管理
extern std::map<uint32_t,asio::ip::tcp::socket> session_map;
extern uint32_t sessionId;
extern std::mutex sessionIDMutex;

extern std::vector<uint8_t> head_buffer;
extern std::vector<uint8_t> temp_buffer;
extern std::vector<uint8_t> total_buffer;
extern uint64_t bodylength;

void read_data(const asio::error_code &error, std::size_t bytes_transferred, std::vector<uint8_t> &buffer, asio::ip::tcp::socket &socket);
void read_head(const asio::error_code &error, std::size_t bytes_transferred, std::vector<uint8_t> &buffer, asio::ip::tcp::socket &socket);
void connect_handler(const asio::error_code &error, const asio::ip::tcp::endpoint &endpoint, asio::ip::tcp::socket &socket);
void connect_to_server(std::string server_host,std::string port);
void connect_to_server_async(std::string server_host, std::string port,std::function<void()> callback);
void send_message(std::string message);
uint32_t get_sessionid();

#endif