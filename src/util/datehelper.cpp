#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "datehelper.h"

std::string datehelper::get_currentdate()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    // Create the desired format
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y%m%d %H:%M:%S");
    std::string formattedTime = oss.str();
    return formattedTime;
}