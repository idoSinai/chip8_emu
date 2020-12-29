#include <bits/stdint-uintn.h>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <string>

constexpr uint16_t memory_size = 4096;

constexpr uint8_t general_reg_size = 16, 
              display_height = 64,
              display_width = 32,
              stack_size = 16,
              keypad_size = 16; // hex based keypad 0x0-0xF

class Chip8 {
  public:
    
    Chip8(const std::string& path);
    ~Chip8() = default;
    
    void handle_opcode();

  private:
    /* attributes*/
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

    uint8_t _opcode; // saves the current opcode  
    std::unordered_map<uint8_t, void (*)()> _opcode_func; 

    /* methods */
    void init_fonts();
    void load_game(const std::string& path);
    void update_timers();
};


