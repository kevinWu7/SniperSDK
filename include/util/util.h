#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <cstdint> 
#include <filesystem>


class util
{
    public :
    static std::string get_currentdate();
    static uint64_t extractUint64(const std::vector<uint8_t>& bytes);  
    static std::filesystem::path get_executablePath();
};
#endif