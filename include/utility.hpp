#pragma once

#include <ios>
#include <iomanip>
#include <sstream>
#include <cstdint>

namespace utility {
  const std::string get_hex(const uint64_t num, const uint64_t digits) {
    std::stringstream ss;
    ss << "0x" << std::uppercase << std::setfill('0') 
       << std::setw(digits) << std::hex << num;
    return ss.str();
  }
}
