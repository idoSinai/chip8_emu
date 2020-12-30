#include <iostream>
#include <filesystem>
#include <string>
#include <memory>
#include "chip8.hpp"

/**
 * this program emulates a chip8 machine 
 * 
 * @param argv[1] rom's path
 * @return 0 for successful run otherwise 1
 */
int main(int argc, char** argv) {
  if(argc != 2) {
    std::cerr << "[usage]: " << argv[0] << " <ROM file>" << std::endl; 
    return 1;
  }
  else if(!std::filesystem::exists(std::string(argv[1]))) {
    std::cerr << "[PATH]: " << argv[1] << " does not exist" << std::endl;
    return 1;
  }

  Chip8 vm{argv[1]};
  vm.run();

  return 0;
}
