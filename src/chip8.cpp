#include <fstream>
#include <array>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <iostream>
#include "..//include//chip8.hpp"


constexpr uint16_t program_start_addr = 0x200;
constexpr uint8_t fonts_size = 80,
                  scale_factor = 10;

Chip8::Chip8(const std::string& path) : 
            _graphics(display_width, display_height, scale_factor, path) 
{
  _memory.fill(0);
  _stack.fill(0);
  _keypad.fill(0);

  std::memset(&_reg, 0, sizeof(_reg));
  std::memset(&_timer, 0, sizeof(_timer));
  std::memset(&_display, 0, sizeof(_display));

  _opcode = 0;
  _reg.pc = program_start_addr;
  
  init_fonts();
  load_game(path);
}


void Chip8::handle_opcode() {
  _opcode = _memory[_reg.pc] << 8 | _memory[_reg.pc + 1];

  if(_opcode_func.find(_opcode) != _opcode_func.cend())
    _opcode_func[_opcode]();
  else
   std::cerr << "no such opcode exist" << std::endl;

  update_timers();  
}


void Chip8::init_fonts() {
  std::array<uint8_t, fonts_size> fonts { {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F 
    } };
  std::copy(fonts.begin(), fonts.end(), _memory.begin());
}


void Chip8::load_game(const std::string& path) { 
  std::ifstream game_file(path);
  if(!game_file)
    throw std::invalid_argument("bad file path");

  uint8_t val;
  uint16_t i = program_start_addr;
  while(game_file >> val) 
    _memory[i++] = val;
}


void Chip8::update_timers() {
  if(_timer.delay > 0)
    _timer.delay--;

  if(_timer.sound > 0) {
    if(_timer.sound == 1)
      std::cout << "BOOP!" << std::endl;
    _timer.sound--;
  }
}




