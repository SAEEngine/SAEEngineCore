#ifndef SAE_ENGINE_CORE_WIDGET_H
#define SAE_ENGINE_CORE_WIDGET_H

#include <concepts>
#include <cstdint>
#include <numeric>
#include <vector>

namespace sae::engine::core {
struct pixels_t {
  using value_type = int16_t;

  friend inline constexpr bool operator==(pixels_t _lhs,
                                          pixels_t _rhs) noexcept {
    return _lhs.count == _rhs.count;
  };
  friend inline constexpr bool operator!=(pixels_t _lhs,
                                          pixels_t _rhs) noexcept {
    return _lhs.count != _rhs.count;
  };

  friend inline constexpr bool operator>(pixels_t _lhs,
                                         pixels_t _rhs) noexcept {
    return _lhs.count > _rhs.count;
  };
  friend inline constexpr bool operator<(pixels_t _lhs,
                                         pixels_t _rhs) noexcept {
    return _lhs.count < _rhs.count;
  };
  friend inline constexpr bool operator>=(pixels_t _lhs,
                                          pixels_t _rhs) noexcept {
    return _lhs.count >= _rhs.count;
  };
  friend inline constexpr bool operator<=(pixels_t _lhs,
                                          pixels_t _rhs) noexcept {
    return _lhs.count <= _rhs.count;
  };

  friend inline constexpr pixels_t operator+(pixels_t _lhs,
                                             pixels_t _rhs) noexcept {
    return pixels_t{_lhs.count + _rhs.count};
  };
  friend inline pixels_t &operator+=(pixels_t &_lhs, pixels_t _rhs) noexcept {
    _lhs.count += _rhs.count;
    return _lhs;
  };

  friend inline constexpr pixels_t operator-(pixels_t _lhs,
                                             pixels_t _rhs) noexcept {
    return pixels_t{_lhs.count - _rhs.count};
  };
  friend inline pixels_t &operator-=(pixels_t &_lhs, pixels_t _rhs) noexcept {
    _lhs.count -= _rhs.count;
    return _lhs;
  };

  friend inline constexpr pixels_t operator*(pixels_t _lhs,
                                             pixels_t _rhs) noexcept {
    return pixels_t{_lhs.count * _rhs.count};
  };
  friend inline pixels_t &operator*=(pixels_t &_lhs, pixels_t _rhs) noexcept {
    _lhs.count *= _rhs.count;
    return _lhs;
  };

  friend inline constexpr pixels_t operator/(pixels_t _lhs,
                                             pixels_t _rhs) noexcept {
    return pixels_t{_lhs.count / _rhs.count};
  };
  friend inline pixels_t &operator/=(pixels_t &_lhs, pixels_t _rhs) noexcept {
    _lhs.count /= _rhs.count;
    return _lhs;
  };

  friend inline constexpr pixels_t operator+(pixels_t _lhs, int _rhs) noexcept {
    return pixels_t{(value_type)(_lhs.count + _rhs)};
  };
  friend inline pixels_t &operator+=(pixels_t &_lhs, int _rhs) noexcept {
    _lhs.count += (value_type)_rhs;
    return _lhs;
  };

  friend inline constexpr pixels_t operator-(pixels_t _lhs, int _rhs) noexcept {
    return pixels_t{(value_type)(_lhs.count - _rhs)};
  };
  friend inline pixels_t &operator-=(pixels_t &_lhs, int _rhs) noexcept {
    _lhs.count -= (value_type)_rhs;
    return _lhs;
  };

  friend inline constexpr pixels_t operator*(pixels_t _lhs, int _rhs) noexcept {
    return pixels_t{_lhs.count * (value_type)_rhs};
  };
  friend inline pixels_t &operator*=(pixels_t &_lhs, int _rhs) noexcept {
    _lhs.count *= (value_type)_rhs;
    return _lhs;
  };

  friend inline constexpr pixels_t operator/(pixels_t _lhs, int _rhs) noexcept {
    return pixels_t{_lhs.count / (value_type)_rhs};
  };
  friend inline pixels_t &operator/=(pixels_t &_lhs, int _rhs) noexcept {
    _lhs.count /= (value_type)_rhs;
    return _lhs;
  };

  friend inline pixels_t operator~(pixels_t _lhs) noexcept {
    return {_lhs.count};
  };

  friend inline constexpr pixels_t operator-(pixels_t _lhs) noexcept {
    return pixels_t{-_lhs.count};
  };

  template <std::floating_point T>
  constexpr explicit operator T() const noexcept {
    return (T)this->count;
  };
  template <std::integral T> constexpr explicit operator T() const noexcept {
    return (T)this->count;
  };

  constexpr pixels_t() noexcept = default;

  template <std::integral T>
  constexpr pixels_t(T _v) noexcept : count{(value_type)_v} {};
  template <std::integral T> pixels_t &operator=(T _v) noexcept {
    this->count = (value_type)_v;
    return *this;
  };

  value_type count = 0;
};

constexpr static inline pixels_t operator""_px(unsigned long long n) {
  return pixels_t{(pixels_t::value_type)n};
};
}; // namespace sae::engine::core

namespace std {
constexpr static inline auto
midpoint(sae::engine::core::pixels_t _lhs,
         sae::engine::core::pixels_t _rhs) noexcept {
  return midpoint(_lhs.count, _rhs.count);
};
}; // namespace std

namespace sae::engine::core {

/**
 * @brief POD type for resprenting a point on the screen (given in pixels;
 * origin is upper left of window)
 */
struct ScreenPoint {
  pixels_t x = 0_px;
  pixels_t y = 0_px;
};

/**
 * @brief POD type for representing a region on the screen in pixels
 */
struct Rect {

  // The upper left point of the region
  ScreenPoint a;

  // The lower right point of the region
  ScreenPoint b;

  /**
   * @brief Returns a reference to the position of the left side of the
   * rectangle
   */
  pixels_t &left() noexcept { return this->a.x; };

  /**
   * @brief Returns a const reference the position of the left side of the
   * rectangle
   */
  constexpr const pixels_t &left() const noexcept { return this->a.x; };

  /**
   * @brief Returns a reference to the position of the right side of the
   * rectangle
   */
  pixels_t &right() noexcept { return this->b.x; };

  /**
   * @brief Returns a const reference the position of the right side of the
   * rectangle
   */
  constexpr const pixels_t &right() const noexcept { return this->b.x; };

  /**
   * @brief Returns a reference to the position of the top side of the rectangle
   */
  pixels_t &top() noexcept { return this->a.y; };

  /**
   * @brief Returns a const reference the position of the top side of the
   * rectangle
   */
  constexpr const pixels_t &top() const noexcept { return this->a.y; };

  /**
   * @brief Returns a reference to the position of bottom top side of the
   * rectangle
   */
  pixels_t &bottom() noexcept { return this->b.y; };

  /**
   * @brief Returns a const reference the position of the bottom side of the
   * rectangle
   */
  constexpr const pixels_t &bottom() const noexcept { return this->b.y; };

  /**
   * @brief Returns the width of the region
   * @return value in pixels
   */
  constexpr pixels_t width() const noexcept {
    return this->right() - this->left();
  };

  /**
   * @brief Returns the height of the region
   * @return value in pixels
   */
  constexpr pixels_t height() const noexcept {
    return this->bottom() - this->top();
  };

  /**
   * @brief Shifts the region defined by this rect by _dx horizontally, and _dy
   * and vertically
   * @param _dx Change in x position
   * @param _dy Change in y position
   */
  Rect &shift(pixels_t _dx, pixels_t _dy) noexcept;

  /**
   * @brief Changes the width of the rectangle by _dw * 2 and the height by _dh
   * * 2. The center of the rectangle doesn't move.
   * @param _dw Change in width (actual change would be _dw * 2)
   * @param _dh Change in height (actual change would be _dw * 2)
   */
  Rect &grow(pixels_t _dw, pixels_t _dh) noexcept;

  /**
   * @brief Checks if a point on the screen intersects this rectangle
   * @param _p Point
   * @return true if intersecting, false if not
   */
  constexpr bool intersects(ScreenPoint _p) const noexcept {
    return (this->left() <= _p.x && _p.x < this->right() &&
            this->top() <= _p.y && _p.y < this->bottom());
  };

  /**
   * @brief Returns the position of the center of the rectangle
   * @return ScreenPoint (pair of positions in pixels)
   */
  constexpr ScreenPoint find_center() const noexcept {
    return ScreenPoint{std::midpoint(this->left(), this->right()),
                       std::midpoint(this->top(), this->bottom())};
  };
};

/**
 * @brief Represents an R color with 8 bits for each color value (8 for the full
 * color)
 */
union ColorR_8 {
  using value_type = uint8_t;
  constexpr static inline size_t count = 4;
  struct {
    uint8_t r;
  };
  uint8_t col[count]{};
};

/**
 * @brief Represents an RGB color with 8 bits for each color value (24 for the
 * full color)
 */
union ColorRGB_8 {
  using value_type = uint8_t;
  constexpr static inline size_t count = 3;
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };
  uint8_t col[count]{};
};

/**
 * @brief Represents an RGBA color with 8 bits for each color value (32 for the
 * full color)
 */
union ColorRGBA_8 {
  using value_type = uint8_t;
  constexpr static inline size_t count = 4;
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
  uint8_t col[count]{};
};

struct Palette {
public:
  using color_type = ColorRGBA_8;
  using value_type = color_type;

  using pointer = value_type *;
  using reference = value_type &;
  using const_pointer = const value_type *;
  using const_reference = const value_type &;

private:
  using container_type = std::vector<value_type>;

public:
  using size_type = size_t;
  using allocator = std::allocator<value_type>;

  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

  size_type size() const noexcept { return this->data_.size(); };

  pointer data() noexcept { return this->data_.data(); };
  const_pointer data() const noexcept { return this->data_.data(); };

  iterator begin() noexcept { return this->data_.begin(); };
  const_iterator begin() const noexcept { return this->data_.cbegin(); };
  const_iterator cbegin() const noexcept { return this->data_.cbegin(); };

  iterator end() noexcept { return this->data_.end(); };
  const_iterator end() const noexcept { return this->data_.cend(); };
  const_iterator cend() const noexcept { return this->data_.cend(); };

  reference at(size_type i) { return this->data_.at(i); };
  const_reference at(size_type i) const { return this->data_.at(i); };

  reference operator[](size_type i) { return this->at(i); };
  const_reference operator[](size_type i) const { return this->at(i); };

  void assign(iterator _at, const_iterator _begin, const_iterator _end) {
    std::copy(_begin, _end, _at);
  };
  void assign(const_iterator _begin, const_iterator _end) {
    this->assign(this->begin(), _begin, _end);
  };

  void assign(size_type _at, const Palette &_other) {
    this->assign(this->begin() + _at, _other.begin(), _other.end());
  };
  void assign(const Palette &_other) { this->assign(0, _other); };

  Palette(size_type _size) : data_(_size){};
  Palette(std::initializer_list<value_type> _colors) : data_{_colors} {};

  Palette(const Palette &_other) = default;
  Palette &operator=(const Palette &_other) = delete;

  Palette(Palette &&_other) noexcept = delete;
  Palette &operator=(Palette &&_other) noexcept = delete;

private:
  container_type data_{};
};

} // namespace sae::engine::core

#endif
