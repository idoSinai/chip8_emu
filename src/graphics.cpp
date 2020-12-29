#include "../include/graphics.hpp"


Graphics::Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog_name) :
                  _scale_factor(scale_factor),
                  window(sf::VideoMode(width * scale_factor, height * scale_factor), prog_name)
{
}


template<uint8_t width, uint8_t height>
void Graphics::draw_window(const std::array<std::array<uint8_t, width>, height>& screen) {
  window.clear(sf::Color::Black);
  sf::RectangleShape pxl(sf::Vector2f(_scale_factor, _scale_factor));
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      if(screen[i][j]) {
        pxl.setPosition(j * _scale_factor, i * _scale_factor);
        pxl.setFillColor(sf::Color::White);
        window.draw(pxl);
      }
    }
  }
  window.display();
}

