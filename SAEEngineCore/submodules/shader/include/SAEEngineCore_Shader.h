#pragma once
#ifndef SAE_ENGINE_CORE_SHADER_H
#define SAE_ENGINE_CORE_SHADER_H

#include <SAEEngineCore_Environment.h>

#include <istream>
#include <optional>

namespace sae::engine::core {
class GFXContext;

class ShaderProgram {
public:
  bool good() const noexcept;

  GLuint id() const noexcept;
  void destroy();

  void bind();
  void unbind();

  ShaderProgram(const ShaderProgram &other) = delete;
  ShaderProgram &operator=(const ShaderProgram &other) = delete;

  ShaderProgram(ShaderProgram &&other);
  ShaderProgram &operator=(ShaderProgram &&other);

protected:
  explicit ShaderProgram(GLuint _id);

private:
  GLuint id_ = 0;
  friend std::optional<ShaderProgram>
  HACK_generate_shader(std::istream &_vertex, std::istream &_fragment);
};

class ShaderStage {
public:
  bool good() const noexcept;

  GLuint id() const noexcept;
  void destroy();

private:
  GLuint id_ = 0;
};

[[deprecated("this is a temporary hack because im lazy as shit")]] std::
    optional<ShaderProgram>
    HACK_generate_shader(std::istream &_vertex, std::istream &_fragment);

} // namespace sae::engine::core

#endif