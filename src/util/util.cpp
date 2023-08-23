#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "util.h"

std::string util::get_currentdate()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    // Create the desired format
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y%m%d %H:%M:%S");
    std::string formattedTime = oss.str();
    return formattedTime;
}

uint64_t util::extractUint64(const std::vector<uint8_t>& bytes)  
{
    uint64_t result = 0;
    for (size_t i = 0; i < 8; ++i)
    {
        result = (result << 8) | bytes[i];
    }
    return result;
}