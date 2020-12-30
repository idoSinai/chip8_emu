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

/**
 * drawing the window in white (while the background is black) 
 * based on the values of matrix which represent the screen
 * 
 * @param screen matrix which represent the screen to be drawn
 */
template<uint8_t width, uint8_t height>
void Graphics::draw_window(const std::array<std::array<uint8_t, width>, height>& screen) {
  window.clear(sf::Color::Black);
  sf::RectangleShape pxl(sf::Vector2f(_scale_factor, _scale_factor));
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      if(screen[i][j]) {
        pxl.setPosition(j  * _scale_factor, i * _scale_factor);
        pxl.setFillColor(sf::Color::White);
        window.draw(pxl);
      }
    }
  }
  window.display();
}


