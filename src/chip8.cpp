#include <SFML/Window/Event.hpp>
#include <bits/stdint-uintn.h>
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


void Chip8::run() {
  sf::Event event;
  while(_graphics.window.isOpen()) {
    while(_graphics.window.pollEvent(event)) {
      switch (event.type) {
        /* updating the keypad */
        case sf::Event::Closed:
          _graphics.window.close();
          break;
        case sf::Event::KeyPressed:
          update_key(event, static_cast<uint8_t>(Key_State::PRESSED));
          break;
        case sf::Event::KeyReleased:
          update_key(event, static_cast<uint8_t>(Key_State::RELEASED));
          break;
        default:
          break;
      }  
    }
    handle_opcode();
    update_timers();
  }
}


void Chip8::handle_opcode() {
  _opcode = _memory[_reg.pc] << 8 | _memory[_reg.pc + 1];

  if(_opcode_func.find(_opcode) != _opcode_func.cend())
    _opcode_func[_opcode]();
  else
   std::cerr << "no such opcode exist" << std::endl;
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
    throw std::invalid_argument("can not open ROM");

  uint8_t val;
  uint16_t i = program_start_addr;
  while(game_file >> val) {
    if(i >= _memory.size())
      throw std::overflow_error("ROM file is too big");
    _memory[i++] = val;
  }
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

void Chip8::update_key(const sf::Event& event, const uint8_t state) {
  switch(event.key.code) {
    case sf::Keyboard::Num1:
      _keypad[0] = state;
      break;
    case sf::Keyboard::Num2:   
      _keypad[1] = state; 
      break;
    case sf::Keyboard::Num3:   
      _keypad[2] = state; 
      break;
    case sf::Keyboard::Num4:   
      _keypad[3] = state; 
      break;
    case sf::Keyboard::Q:      
      _keypad[4] = state; 
      break;
    case sf::Keyboard::W:      
      _keypad[5] = state; 
      break;
    case sf::Keyboard::E:      
      _keypad[6] = state; 
      break;
    case sf::Keyboard::R:      
      _keypad[7] = state; 
      break;
    case sf::Keyboard::A:      
      _keypad[8] = state; 
      break;
    case sf::Keyboard::S:      
      _keypad[9] = state; 
      break;
    case sf::Keyboard::D:      
      _keypad[10] = state; 
      break;
    case sf::Keyboard::F:      
      _keypad[11] = state; 
      break;
    case sf::Keyboard::Z:
      _keypad[12] = state; 
      break;
    case sf::Keyboard::X:      
      _keypad[13] = state; 
      break;
    case sf::Keyboard::C:      
      _keypad[14] = state; 
      break;
    case sf::Keyboard::V:
      _keypad[15] = state; 
      break;
    case sf::Keyboard::Escape:
      _graphics.window.close(); 
      break;
    default: 
      break;
  }
}



