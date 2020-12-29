#include "graphics.hpp"


Graphics::Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog_name) :
                  _scale_factor(scale_factor),
                  window(sf::VideoMode(width * scale_factor, height * scale_factor), prog_name)
{
  auto desk { sf::VideoMode::getDesktopMode() };  
  window.setPosition(sf::Vector2i(desk.width / 4, desk.height / 4));
}


