#pragma once
#ifndef SAE_ENGINE_CORE_UI_H
#define SAE_ENGINE_CORE_UI_H

#include <SAEEngineCore_Object.h>

namespace sae::engine::core {
class UIList : public GFXView {
private:
  void reposition_horizontal(pixels_t _eachWidth);
  void reposition_horizontal();
  void reposition_vertical(pixels_t _eachHeight);
  void reposition_vertical();

public:
  enum AXIS { HORIZONTAL, HOR = HORIZONTAL, VERTICAL, VERT = VERTICAL };
  enum DIRECTION { POSITIVE, POS = POSITIVE, NEGATIVE, NEG = NEGATIVE };

  void refresh() override;

  pixels_t &margin() noexcept;
  const pixels_t &margin() const noexcept;

  void set_axis(AXIS _axis) noexcept;
  AXIS axis() const noexcept;

  void set_direction(DIRECTION _dir) noexcept;
  DIRECTION direction() const noexcept;

  bool is_fixed_size() const noexcept;
  void use_fixed_size(Rect _r) noexcept;
  std::optional<Rect> fixed_size() const noexcept;

  UIList(GFXContext *_context, Rect _r, AXIS _axis = AXIS::HORIZONTAL,
         DIRECTION _dir = DIRECTION::POSITIVE, pixels_t _margin = 5_px);

private:
  std::optional<Rect> fixed_size_;
  pixels_t margin_;
  AXIS axis_;
  DIRECTION dir_;
};

} // namespace sae::engine::core

#endif