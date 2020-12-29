#pragma once 

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <array>

class Graphics {
  public:
    Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog_name);    
    Graphics() = delete;
    ~Graphics() = default;

    template<uint8_t width, uint8_t height>
    void draw_window(const std::array<std::array<uint8_t, width>, height>& screen);
    
    sf::RenderWindow window;

  private:
    const uint8_t _scale_factor;
};
