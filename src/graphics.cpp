#include "graphics.hpp"

/* *
 * constructor 
 *
 * @param width width of window to be created
 * @param heigh height of window to be created
 * @param scale_factor scale factor size of the window to be created
 * @param prog_name title of the program
 */
Graphics::Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog_name) :
                  _scale_factor(scale_factor),
                  window(sf::VideoMode(width * scale_factor, height * scale_factor), prog_name)
{
  /* centralize the screen */
  auto desk { sf::VideoMode::getDesktopMode() };  
  window.setPosition(sf::Vector2i(desk.width / 4, desk.height / 4));
}


