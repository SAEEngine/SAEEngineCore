#pragma once
#ifndef SAE_ENGINE_CORE_ARTIST_H
#define SAE_ENGINE_CORE_ARTIST_H

#include <SAEEngineCore_Event.h>

#include <concepts>
#include <type_traits>
#include <unordered_set>

namespace sae::engine::core {
class GFXObject;

class IArtist {
public:
  virtual bool good() = 0;
  virtual void draw() = 0;

  virtual void remove(GFXObject *_obj) = 0;
  virtual bool contains(GFXObject *_obj) const = 0;

  virtual void handle_event(Event &_event){};

  virtual ~IArtist() = default;
};

template <typename T> class Artist;

template <typename T>
concept cx_artist = std::is_base_of<IArtist, T>::value &&requires(
    T a, typename T::art_type *_art) {
  typename T::art_type;
  a.draw();
  { a.good() }
  ->std::same_as<bool>;
  a.remove((GFXObject *)_art);
  a.insert(_art);
  a.refresh(_art);
};

} // namespace sae::engine::core

#endif