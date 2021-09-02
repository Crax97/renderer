#pragma once

#include <glad/glad.h>
#include <SDL2/SDL_video.h>
#include "graphics_api.h"

namespace renderer {

class opengl_graphics_api final : public graphics_api {
private:
  SDL_GLContext GLContext;
  SDL_Window *Window;
  GLuint m_unif_block_buffer{};

public:
  explicit opengl_graphics_api(SDL_Window *Window) noexcept;


  std::shared_ptr<mesh> create_mesh(vertices verts, indices ind, normals norms, tex_coords texs) noexcept override;
  std::shared_ptr<shader> compile_shader(const std::string& vertex_shader_src, const std::string& pixel_shader_src) noexcept override;
  std::shared_ptr<class texture> create_texture(unsigned char* data, int width, int height, texture_format format) noexcept override;

  void pre_draw() noexcept override;
  void post_draw() noexcept override;

  ~opengl_graphics_api() noexcept override;

};
} // namespace renderer