#include "opengl_shader.h"
#include "opengl_texture.h"

#include <cassert>
#include <glad/glad.h>

renderer::opengl_shader::opengl_shader(GLuint program) noexcept
    : m_program(program) {
  assert(glIsProgram(m_program));
  glBindAttribLocation(m_program, KNOWN_LOCATIONS::VertexPosition, "vertex");
  glBindAttribLocation(m_program, KNOWN_LOCATIONS::TexcoordPosition,
                       "texcoord");
  glBindAttribLocation(m_program, KNOWN_LOCATIONS::NormalPosition, "normal");
}

void renderer::opengl_shader::use() { glUseProgram(m_program); }

void renderer::opengl_shader::set_uniform_matrix4x4(
    const std::string &uniform_name, float *mat4x4) {
  auto uniform_location = get_uniform_location(uniform_name);
  if (uniform_location == -1) {
    // log error
    return;
  }
  glProgramUniformMatrix4fv(m_program, uniform_location, 1, GL_FALSE, mat4x4);
}

void renderer::opengl_shader::bind_textures(
    std::vector<std::pair<std::string, std::shared_ptr<texture>>> textures) {

  int i = 0;
  for (auto &pair : textures) {
    const auto &[unif_name, texture] = pair;
    auto unif_loc = get_uniform_location(unif_name);
    if (unif_loc == -1) {
      // log error
      continue;
    }
    texture->bind_to_unit(i);
    glProgramUniform1i(m_program, unif_loc, i);
  }
}
GLint renderer::opengl_shader::get_uniform_location(
    const std::string &uniform_name) noexcept {
  auto cached_uniform = uniforms.find(uniform_name);
  if (cached_uniform != uniforms.end()) {
    return cached_uniform->second;
  }

  auto uniform = glGetUniformLocation(m_program, uniform_name.c_str());
  uniforms.insert({uniform_name, uniform});
  return uniform;
}

renderer::opengl_shader::~opengl_shader() noexcept {
  glDeleteProgram(m_program);
}