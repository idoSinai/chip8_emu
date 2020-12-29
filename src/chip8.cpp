#include <algorithm>
#include <fstream>
#include <array>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include "chip8.hpp"


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
  init_opcode_table();

  std::srand(std::time(nullptr)); // use current time as seed for random generator
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
    std::this_thread::sleep_for(std::chrono::microseconds(2000)); // Setting delay 
  }
}


void Chip8::handle_opcode() {
  bool exec_opcode = false;
  _opcode = _memory[_reg.pc] << 8 | _memory[_reg.pc + 1];

  for(auto inst = _opcode_table.begin(); inst != _opcode_table.cend() && !exec_opcode; inst++) {
    if(((inst->first & _opcode) == _opcode) && ((inst->first | _opcode) == inst->first)) {
      init_opcode_args();
      (*this.*(inst->second))();
      exec_opcode = true;
    }
  }
 
  if(exec_opcode) 
    std::cout << "executed: 0x" << std::hex << _opcode << "  at address: 0x" << std::hex << (_reg.pc - 2)<< std::endl;
  else 
    std::cerr << "didn't executed: " << std::hex << _opcode << std::endl;
}


void Chip8::load_game(const std::string& path) { 
  std::ifstream game_file(path, std::ifstream::ate | std::ifstream::binary);
  if(!game_file)
    throw std::invalid_argument("can not open ROM");
  if(game_file.tellg() > (_memory.size() - program_start_addr))
    throw std::overflow_error("file too big");
  game_file.seekg(std::ifstream::beg);
  game_file.read(reinterpret_cast<char*>(_memory.data() + program_start_addr), memory_size);  
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

void Chip8::init_opcode_args() {
  _opcode_args.nnn = 0x0FFF & _opcode;
  _opcode_args.nn = 0x00FF & _opcode;
  _opcode_args.n = 0x000F & _opcode;
  _opcode_args.x = (0x0F00 & _opcode) >> 8;
  _opcode_args.y = (0x00F0 & _opcode) >> 4;
}

void Chip8::init_opcode_table() {
  _opcode_table[0x00E0] = &Chip8::inst_00E0;
  _opcode_table[0x00EE] = &Chip8::inst_00EE;
  _opcode_table[0x1FFF] = &Chip8::inst_1NNN;
  _opcode_table[0x2FFF] = &Chip8::inst_2NNN;
  _opcode_table[0x3FFF] = &Chip8::inst_3XNN;
  _opcode_table[0x4FFF] = &Chip8::inst_4XNN;
  _opcode_table[0x5FF0] = &Chip8::inst_5XY0;
  _opcode_table[0x6FFF] = &Chip8::inst_6XNN;
  _opcode_table[0x7FFF] = &Chip8::inst_7XNN;
  _opcode_table[0x8FF0] = &Chip8::inst_8XY0;
  _opcode_table[0x8FF1] = &Chip8::inst_8XY1;
  _opcode_table[0x8FF2] = &Chip8::inst_8XY2;
  _opcode_table[0x8FF3] = &Chip8::inst_8XY3;
  _opcode_table[0x8FF4] = &Chip8::inst_8XY4;
  _opcode_table[0x8FF5] = &Chip8::inst_8XY5;
  _opcode_table[0x8FF6] = &Chip8::inst_8XY6;
  _opcode_table[0x8FF7] = &Chip8::inst_8XY7;
  _opcode_table[0x8FFE] = &Chip8::inst_8XYE;
  _opcode_table[0x9FF0] = &Chip8::inst_9XY0; 
  _opcode_table[0xAFFF] = &Chip8::inst_ANNN;
  _opcode_table[0xBFFF] = &Chip8::inst_BNNN;
  _opcode_table[0xCFFF] = &Chip8::inst_CXNN;
  _opcode_table[0xDFFF] = &Chip8::inst_DXYN;
  _opcode_table[0xEF9E] = &Chip8::inst_EX9E;
  _opcode_table[0xEFA1] = &Chip8::inst_EXA1;
  _opcode_table[0xFF07] = &Chip8::inst_FX07;
  _opcode_table[0xFF0A] = &Chip8::inst_FX0A;
  _opcode_table[0xFF15] = &Chip8::inst_FX15;
  _opcode_table[0xFF18] = &Chip8::inst_FX18;
  _opcode_table[0xFF1E] = &Chip8::inst_FX1E;
  _opcode_table[0xFF29] = &Chip8::inst_FX29;
  _opcode_table[0xFF33] = &Chip8::inst_FX33;
  _opcode_table[0xFF55] = &Chip8::inst_FX55;
  _opcode_table[0xFF65] = &Chip8::inst_FX65;
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


void Chip8::update_key(const sf::Event& event, const uint8_t state) {
  switch(event.key.code) {
    case sf::Keyboard::Num1:
      _keypad[1] = state;
      break;
    case sf::Keyboard::Num2:   
      _keypad[2] = state; 
      break;
    case sf::Keyboard::Num3:   
      _keypad[3] = state; 
      break;
    case sf::Keyboard::Num4:   
      _keypad[12] = state; 
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
      _keypad[13] = state; 
      break;
    case sf::Keyboard::A:      
      _keypad[7] = state; 
      break;
    case sf::Keyboard::S:      
      _keypad[8] = state; 
      break;
    case sf::Keyboard::D:      
      _keypad[9] = state; 
      break;
    case sf::Keyboard::F:      
      _keypad[14] = state; 
      break;
    case sf::Keyboard::Z:
      _keypad[10] = state; 
      break;
    case sf::Keyboard::X:      
      _keypad[0] = state; 
      break;
    case sf::Keyboard::C:      
      _keypad[11] = state; 
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


// ==================================== OPCODE DECODING METHODS ======================================  

/*
 * clears the screen 
*/
void Chip8::inst_00E0() {
  std::memset(&_display, 0, sizeof(_display));
  _reg.pc += 2;
}

/*
 * returns from a subroutine
*/
inline void Chip8::inst_00EE() {
  if(_reg.sp > 0) {
    _reg.pc = _stack[--_reg.sp];
    _reg.pc += 2;
  }
  else {
    std::cerr << "Stack overflow" << std::endl;
    exit(1);
  }
}

/*
 * jumps to address NNN
*/
inline void Chip8::inst_1NNN() {
  _reg.pc = _opcode_args.nnn;
}

/*
 * calls subroutine at NNN 
*/ 
inline void Chip8::inst_2NNN() {
  if(_reg.sp < (stack_size - 1)) {
    _stack[_reg.sp++] = _reg.pc;
    _reg.pc = _opcode & 0x0FFF;
  }
  else {
    std::cerr << "Stack overflow" << std::endl;
    exit(1);
  }
}

/*
 * skips the next instruction if VX equals NN
*/
inline void Chip8::inst_3XNN() {
  if(_opcode_args.nn == _reg.V[_opcode_args.x])
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * skips the next instruction if VX doesn't equal NN 
*/
inline void Chip8::inst_4XNN() {
  if(_opcode_args.nn != _reg.V[_opcode_args.x])
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * skips the next instruction if VX equals VY
*/
inline void Chip8::inst_5XY0() {
  if(_reg.V[_opcode_args.x] == _reg.V[_opcode_args.y])
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * sets VX to NN
*/
inline void Chip8::inst_6XNN() {
  _reg.V[_opcode_args.x] = _opcode_args.nn;
  _reg.pc += 2;
}

/*
 * adds NN to VX (carry flag is not changed)
*/
inline void Chip8::inst_7XNN() {
  _reg.V[_opcode_args.x] += _opcode_args.nn;
  _reg.pc += 2;
}

/*
 * sets VX to the value of VY.
*/
inline void Chip8::inst_8XY0() {
  _reg.V[_opcode_args.x] = _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * sets VX to VX or VY
*/
inline void Chip8::inst_8XY1() {
  _reg.V[_opcode_args.x] |= _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * sets VX to VX and VY
*/
inline void Chip8::inst_8XY2() {
  _reg.V[_opcode_args.x] &= _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * sets VX to VX xor VY
*/
inline void Chip8::inst_8XY3() {
  _reg.V[_opcode_args.x] ^= _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
*/
inline void Chip8::inst_8XY4() {
  _reg.V[0xF] = (_reg.V[_opcode_args.x] + _reg.V[_opcode_args.y]) > 0xFFFF ? 1 : 0;   
  _reg.V[_opcode_args.x] += _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
*/ 
inline void Chip8::inst_8XY5() {
  _reg.V[0xF] = _reg.V[_opcode_args.x] < _reg.V[_opcode_args.y] ? 1 : 0;
  _reg.V[_opcode_args.x] -= _reg.V[_opcode_args.y];
  _reg.pc += 2;
}

/*
 * stores the least significant bit of VX in VF and then shifts VX to the right by 1
*/ 
inline void Chip8::inst_8XY6() {
  _reg.V[0xF] = _reg.V[_opcode_args.x] & 0x1u;
  _reg.V[_opcode_args.x] >>= 1;
  _reg.pc += 2;
}

/*
 * sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
*/
inline void Chip8::inst_8XY7() {
  _reg.V[0xF] = _reg.V[_opcode_args.x] > _reg.V[_opcode_args.y] ? 1 : 0;
  _reg.V[_opcode_args.x] = _reg.V[_opcode_args.y] - _reg.V[_opcode_args.x];
  _reg.pc += 2;
}

/*
 * stores the most significant bit of VX in VF and then shifts VX to the left by 1
*/ 
inline void Chip8::inst_8XYE() {
  _reg.V[0xF] = _reg.V[_opcode_args.x] >> 7;
  _reg.V[_opcode_args.x] <<= 1;
  _reg.pc += 2;
}

/*
 * skips the next instruction if VX doesn't equal VY
*/
inline void Chip8::inst_9XY0() {
  if(_reg.V[_opcode_args.x] != _reg.V[_opcode_args.y])
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * sets I to the address NNN
*/
inline void Chip8::inst_ANNN() {
  _reg.idx = _opcode_args.nnn;
  _reg.pc += 2;
}

/*
 * jumps to the address NNN plus V0
*/
inline void Chip8::inst_BNNN() {
  _reg.pc = _opcode_args.nnn + _reg.V[0x0];
}

/*
 * sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
*/
inline void Chip8::inst_CXNN() {
  _reg.V[_opcode_args.x] = (std::rand() % 256) & _opcode_args.nn;
  _reg.pc += 2;
}

/*
 * draws a sprite at coordinate (VX, VY) that has a width of 8 pixels 
 * and a height of N+1 pixels. Each row of 8 pixels is read as bit-coded 
 * starting from memory location I; I value doesn’t change after the execution of this instruction. 
 * As described above, 
 * VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
 * and to 0 if that doesn’t happen
*/
inline void Chip8::inst_DXYN() { 
  // preventing dispay overflow
  uint8_t coord_x = _reg.V[(0x0F00 & _opcode) >> 8] % 64,
          coord_y = _reg.V[(0x00F0 & _opcode) >> 4] % 32,
          sprite_height = 0x000F & _opcode;
  // default state - no collision
  _reg.V[0xf] = 0;
  for (uint8_t row = 0; row < sprite_height; row++) {
      uint8_t px_to_draw = _memory[_reg.idx + row]; // pixel to draw on the screen
      for (uint8_t bit_pos {}; bit_pos < 8; bit_pos++) {
          uint8_t& curr_px = _display[coord_y + row][coord_x + bit_pos]; // current pixel on the screen (can be on or off)
          uint8_t sprite_px = (px_to_draw >> (7 - bit_pos)) & 0x1u;
          // if both pixels are on -> collision has been occured
          if (curr_px && sprite_px) {
              _reg.V[0xf] = 1;
          }
          // either set or off the pixel on the actual display matrix
          curr_px ^= sprite_px;
      }
  }
  _graphics.draw_window<display_width, display_height>(_display);
  _reg.pc += 2;
}

/*
 * skips the next instruction if the key stored in VX is pressed
*/ 
inline void Chip8::inst_EX9E() {
  if(_keypad[_reg.V[_opcode_args.x]] == static_cast<uint16_t>(Key_State::PRESSED))
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * skips the next instruction if the key stored in VX isn't pressed
*/
inline void Chip8::inst_EXA1() {
  if(_keypad[_reg.V[_opcode_args.x]] == static_cast<uint16_t>(Key_State::RELEASED))
    _reg.pc += 2;
  _reg.pc += 2;
}

/*
 * sets VX to the value of the delay timer
*/
inline void Chip8::inst_FX07() {
  _reg.V[_opcode_args.x] = _timer.delay;
  _reg.pc += 2;
}

/*
 * a key press is awaited, and then stored in VX
*/
inline void Chip8::inst_FX0A() {
  for(uint8_t i = 0; i < _keypad.size() ; i++) {
    if(_keypad[i]) {
      _reg.V[_opcode_args.x] = i;
      _reg.pc += 2;
      return;
    }
  }
}

/*
 * sets the delay timer to VX
*/ 
inline void Chip8::inst_FX15() {
  _timer.delay = _reg.V[_opcode_args.x];
  _reg.pc += 2;
}

/*
 * sets the sound timer to VX
*/
inline void Chip8::inst_FX18() {
  _timer.sound = _reg.V[_opcode_args.x];
  _reg.pc += 2;
}

/*
 * adds VX to I
*/
inline void Chip8::inst_FX1E() {
  _reg.V[0xF] = (_reg.idx + _reg.V[_opcode_args.x]) > 0xFFFF ? 1 : 0;
  _reg.idx += _reg.V[_opcode_args.x];
  _reg.pc += 2;
}

/*
 * sets I to the location of the sprite for the character in VX.
 * Characters 0-F (in hexadecimal) are represented by a 4x5 fon
*/ 
inline void Chip8::inst_FX29() {
  _reg.idx = _reg.V[_opcode_args.x] * 5;
  _reg.pc += 2;
}

/*
 * stores the binary-coded decimal representation of VX, 
* with the most significant of three digits at the address in I,
* the middle digit at I plus 1, and the least significant digit at I plus 2
*/
inline void Chip8::inst_FX33() {
  _memory[_reg.idx]     = _reg.V[_opcode_args.x] / 100;
  _memory[_reg.idx + 1] = (_reg.V[_opcode_args.x] / 10) % 10;
  _memory[_reg.idx + 2] = _reg.V[_opcode_args.x] % 10;
  _reg.pc += 2;
}

/*
 * stores V0 to VX (including VX) in memory starting at address I.
 * The offset from I is increased by 1 for each value written, but I itself is left unmodified
*/
inline void Chip8::inst_FX55() {
  for(uint8_t i = 0; i <= _opcode_args.x; i++)
    _memory[_reg.idx + i] = _reg.V[i];
  _reg.pc += 2;
}

/*
 * Fills V0 to VX (including VX) with values from memory starting at address I. 
 * The offset from I is increased by 1 for each value written, but I itself is left unmodified.[
*/ 
inline void Chip8::inst_FX65() {
  for(uint8_t i = 0; i <= _opcode_args.x; i++)
    _reg.V[i] = _memory[_reg.idx + i];
  _reg.pc += 2;
}

