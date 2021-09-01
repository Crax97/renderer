#pragma once

#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <memory>

struct SDL_Window;

namespace renderer {

class graphics_api {
public:
  virtual ~graphics_api() noexcept = default;

  virtual std::shared_ptr<mesh> create_mesh(vertices verts, indices ind, normals norms, tex_coords texs) noexcept = 0;
  virtual std::shared_ptr<shader> compile_shader(const std::string& vertex_shader_src, const std::string& pixel_shader_src) noexcept = 0;
  virtual std::shared_ptr<class texture> create_texture(unsigned char* data, int width, int height, texture_format format) noexcept = 0;

  virtual void pre_draw() noexcept = 0;
  virtual void post_draw() noexcept = 0;
};
} // namespace renderer