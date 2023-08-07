#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "asio.hpp"
#include "src/zlib.h"
#include "baseinfo.h"


void read_handler(const asio::error_code& error,
                  std::size_t bytes_transferred,
                  std::array<char, 1024>& buffer,
                  asio::ip::tcp::socket& socket) {

  if (!error) {
    // Process the received data
    std::string response(buffer.data(), bytes_transferred);
    std::cout << "Server response: " << response << std::endl;
    
    // Continue reading for more data
    socket.async_read_some(asio::buffer(buffer), 
        std::bind(read_handler, std::placeholders::_1, std::placeholders::_2, 
                  std::ref(buffer), std::ref(socket)));
  } 
  else if (error == asio::error::eof) {
    std::cerr << "Server closed the connection" << std::endl;
  }
  else {
    std::cerr << "Read error: " << error.message() << std::endl;
  }
}

void connect_handler(const asio::error_code& error, const asio::ip::tcp::endpoint& endpoint, asio::ip::tcp::socket& socket) {
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
    std::string message = "Hello from client!"+formattedTime+"\n";
    asio::write(socket, asio::buffer(message));
    std::cout << "Send message: " << message << std::endl;
    

    
   /*std::array<char, 1024> buffer;  
    asio::async_read(socket, asio::buffer(buffer),
      std::bind(read_handler, std::placeholders::_1, std::placeholders::_2, std::ref(buffer), std::ref(socket)));
      */
   // 使用同步read_some后，就能正常收到消息了
    char buffer[128];
    std::size_t bytesRead = socket.read_some(asio::buffer(buffer));
    std::string response(buffer, bytesRead);
    std::cout << "服务器返回的消息: " << response << std::endl;
  } 
  else 
  {
    std::cerr << "Connect error: " << error.message() << std::endl;
  }
}

void start_client() {
  try {
    // Create an io_context object
    asio::io_context io_context;
    std::string str=u8"aaa" ;
    // Create a TCP socket object
    asio::ip::tcp::socket socket(io_context);

    // Resolve the server address and port
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server_host, "18889");

    // Connect to the server
    asio::async_connect(socket, endpoints, std::bind(connect_handler,
        std::placeholders::_1, std::placeholders::_2, std::ref(socket)));

    // Run the io_context to start processing the asynchronous operations
    io_context.run();
    std::cout << "IO context finished." << std::endl;
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}

void maycompress()
{
 const unsigned char pData[] = {
		123, 34, 37, 83, 24, 2, 98, 178, 57, 220,
		123, 34, 37, 83, 24, 2, 98, 178, 57, 220,
		123, 34, 37, 83, 24, 2, 98, 178, 57, 220,
		123, 34, 37, 83, 24, 2, 98, 178, 57, 220
	};
	unsigned long nDataSize = 100;

	//printf("Initial size: %d\n", nDataSize);

	unsigned long nCompressedDataSize = nDataSize;
	unsigned char * pCompressedData = new unsigned char[nCompressedDataSize];
	
	int nResult = compress2(pCompressedData, &nCompressedDataSize, pData, nDataSize, 9);

	if (nResult == Z_OK)
	{
		//printf("Compressed size: %d\n", nCompressedDataSize);

		unsigned char * pUncompressedData = new unsigned char[nDataSize];
		nResult = uncompress(pUncompressedData, &nDataSize, pCompressedData, nCompressedDataSize);
		if (nResult == Z_OK)
		{
			//printf("Uncompressed size: %d\n", nDataSize);
			if (memcmp(pUncompressedData, pData, nDataSize) == 0)
				printf("Great Success\n");
		}
		delete [] pUncompressedData;
	}

	delete [] pCompressedData;
}



int main()
{
  maycompress();
  start_client();
  std::cout << "Reached the end" << std::endl;
  return 0;
}

