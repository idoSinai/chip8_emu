#pragma once
#include <cstdint>
#include <array>
#include <unordered_map>
#include <string>
#include "graphics.hpp"

constexpr uint16_t memory_size = 4096;

constexpr uint8_t general_reg_size = 16, 
              display_height = 32,
              display_width = 64,
              stack_size = 16,
              keypad_size = 16; // hex based keypad 0x0-0xF

enum class Key_State : uint8_t {RELEASED = 0, PRESSED = 1};

class Chip8 {
  public:
    
    Chip8(const std::string& path);
    Chip8() = delete;
    ~Chip8() = default;
    
    void run();

  private:
    /* attributes*/
    Graphics _graphics;
    std::array<uint8_t, memory_size> _memory; // chip8's memory space  

    std::array<uint16_t, stack_size> _stack;
    /* cpu registers */
    struct {
      std::array<uint8_t, general_reg_size> V; // general purpose registers named V0 up to VF
      uint16_t idx; 
      uint16_t pc; // program counter
      uint16_t sp; 
    } _reg;

    std::array<std::array<uint8_t, display_width>, display_height> _display; // represent the black and white screen, hold the pixel state(1/0)
    std::array<uint8_t, keypad_size> _keypad;
    /* timer registers, when set above zero they will count down to zero */ 
    struct {
      uint8_t delay;
      uint8_t sound;      
    } _timer;

    uint16_t _opcode; // saves the current opcode
    typedef void (Chip8::*inst_func)();
    std::unordered_map<uint16_t, inst_func> _opcode_table; 

    /* methods */
    void handle_opcode();
    void init_fonts();
    void init_opcode_table();
    void load_game(const std::string& path);
    void update_timers();
    void update_key(const sf::Event& event, const uint8_t state);


    /* instructions' methods for executing opcodes */
    /* For more information: wikipedia.org/wiki/CHIP-8#Opcode_table */
    void inst_0NNN() {};
    void inst_00E0() {};
    void inst_00EE() {};
    void inst_1NNN() {};
    void inst_2NNN() {};
    void inst_3XNN() {};
    void inst_4XNN() {};
    void inst_5XY0() {};
    void inst_6XNN() {};
    void inst_7XNN() {};
    void inst_8XY0() {};
    void inst_8XY1() {};
    void inst_8XY2() {};
    void inst_8XY3() {};
    void inst_8XY4() {};
    void inst_8XY5() {};
    void inst_8XY6() {};
    void inst_8XY7() {};
    void inst_8XYE() {};
    void inst_9XY0() {};
    void inst_ANNN() {};
    void inst_BNNN() {};
    void inst_CXNN() {};
    void inst_DXYN() {};
    void inst_EX9E() {};
    void inst_EXA1() {};
    void inst_FX07() {};
    void inst_FX0A() {};
    void inst_FX15() {};
    void inst_FX18() {};
    void inst_FX1E() {};
    void inst_FX29() {};
    void inst_FX33() {};
    void inst_FX55() {};
    void inst_FX65() {};
};


