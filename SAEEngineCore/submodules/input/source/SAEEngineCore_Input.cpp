#include "SAEEngineCore_Input.h"

#include <GLFW/glfw3.h>

namespace sae::engine::core {

Cursor::Position Cursor::get_position(GLFWwindow *_window) {
  double _xpos = 0.0;
  double _ypos = 0.0f;
  glfwGetCursorPos(_window, &_xpos, &_ypos);
  return Position{(int16_t)_xpos, (int16_t)_ypos};
};

} // namespace sae::engine::core
