#include "SAEEngineCore_Widget.h"

namespace sae::engine::core {
Rect &Rect::shift(pixels_t _dx, pixels_t _dy) noexcept {
  this->left() += _dx;
  this->right() += _dx;

  this->top() += _dy;
  this->bottom() += _dy;

  return *this;
};

Rect &Rect::grow(pixels_t _dw, pixels_t _dh) noexcept {
  this->left() -= _dw;
  this->right() += _dw;

  this->top() -= _dh;
  this->bottom() += _dh;

  return *this;
};
} // namespace sae::engine::core
