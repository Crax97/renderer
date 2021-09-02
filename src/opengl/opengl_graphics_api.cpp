#include "opengl_graphics_api.h"
#include "opengl_mesh.h"
#include "opengl_shader.h"
#include "opengl_texture.h"
#include <SDL2/SDL_video.h>

#include <cassert>
#include <iostream>

#include <glad/glad.h>

// modified from https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f
void APIENTRY gl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                                        GLenum severity, GLsizei,
                                        const GLchar *msg, const void *) {
  std::string_view _source;
  std::string_view _type;
  std::string_view _severity;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    _source = "API";
    break;

  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    _source = "WINDOW SYSTEM";
    break;

  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    _source = "SHADER COMPILER";
    break;

  case GL_DEBUG_SOURCE_THIRD_PARTY:
    _source = "THIRD PARTY";
    break;

  case GL_DEBUG_SOURCE_APPLICATION:
    _source = "APPLICATION";
    break;

  case GL_DEBUG_SOURCE_OTHER:
  default:
    _source = "UNKNOWN";
    break;
  }

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    _type = "ERROR";
    break;

  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    _type = "DEPRECATED BEHAVIOR";
    break;

  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    _type = "UDEFINED BEHAVIOR";
    break;

  case GL_DEBUG_TYPE_PORTABILITY:
    _type = "PORTABILITY";
    break;

  case GL_DEBUG_TYPE_PERFORMANCE:
    _type = "PERFORMANCE";
    break;

  case GL_DEBUG_TYPE_OTHER:
    _type = "OTHER";
    break;

  case GL_DEBUG_TYPE_MARKER:
    _type = "MARKER";
    break;

  default:
    _type = "UNKNOWN";
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    _severity = "HIGH";
    break;

  case GL_DEBUG_SEVERITY_MEDIUM:
    _severity = "MEDIUM";
    break;

  case GL_DEBUG_SEVERITY_LOW:
    _severity = "LOW";
    break;

  case GL_DEBUG_SEVERITY_NOTIFICATION:
    _severity = "NOTIFICATION";
    break;

  default:
    _severity = "UNKNOWN";
    break;
  }

  std::cout << id << ": " << _type << " of " << _severity
            << " severity, raised from " << _source << ": " << msg << "\n";
}

renderer::opengl_graphics_api::opengl_graphics_api(SDL_Window *Window) noexcept {
  assert(Window);
  this->GLContext = SDL_GL_CreateContext(Window);
  this->Window = Window;
  assert(this->GLContext);

  SDL_GL_MakeCurrent(this->Window, this->GLContext);
  assert(gladLoadGL() == 1);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#else
  glEnable(GL_DEBUG_OUTPUT);
#endif

  glClearColor(0.3f, 0.3, 0.3f, 1.0f);
  glGenBuffers(1, &m_unif_block_buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, m_unif_block_buffer);
  glBufferData(GL_UNIFORM_BUFFER, 1024, nullptr, GL_DYNAMIC_DRAW);

  glDebugMessageCallback(&gl_debug_message_callback, nullptr);
}

void renderer::opengl_graphics_api::pre_draw() noexcept {
  SDL_GL_MakeCurrent(this->Window, this->GLContext);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glBindBuffer(GL_UNIFORM_BUFFER, m_unif_block_buffer);
}

void renderer::opengl_graphics_api::post_draw() noexcept {
  SDL_GL_SwapWindow(this->Window);
}
renderer::opengl_graphics_api::~opengl_graphics_api() noexcept {
  SDL_GL_DeleteContext(this->GLContext);
}

std::shared_ptr<renderer::mesh> renderer::opengl_graphics_api::create_mesh(
    renderer::vertices verts, renderer::indices ind, renderer::normals norms,
    renderer::tex_coords texs) noexcept {
  return std::make_shared<opengl_mesh>(verts, texs, norms, ind);
}

std::shared_ptr<renderer::texture> renderer::opengl_graphics_api::create_texture(
    unsigned char *data, int width, int height,
    renderer::texture_format format) noexcept {

  GLenum gl_pixel_format, gl_type;
  GLint gl_internal_format;
  switch (format) {

  case texture_format::rgb:
    gl_internal_format = GL_RGBA;
    gl_type = GL_UNSIGNED_BYTE;
    gl_pixel_format = GL_RGB;
    break;
  case texture_format::rgba:
    gl_internal_format = GL_RGBA;
    gl_type = GL_UNSIGNED_BYTE;
    gl_pixel_format = GL_RGBA;
    break;
  case texture_format::red:
    gl_internal_format = GL_RGBA;
    gl_type = GL_UNSIGNED_BYTE;
    gl_pixel_format = GL_RED;
    break;
  case texture_format::depth_texture:
    gl_internal_format = GL_DEPTH_COMPONENT;
    gl_type = GL_FLOAT;
    gl_pixel_format = GL_DEPTH_COMPONENT32;
    break;
  case texture_format::unknown:
      assert(false);
  }

  GLuint tex_id;
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0,
               gl_pixel_format, gl_type, data);

  if (width == height && data) {
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  return std::make_shared<opengl_texture>(tex_id);
}

std::string shader_type_to_str(GLenum type) {
  switch (type) {
  case GL_VERTEX_SHADER:
    return "Vertex Shader";
  case GL_FRAGMENT_SHADER:
    return "Fragment Shader";
  default:
    return "Unknown Shader";
  }
}

std::optional<GLuint> compile_gl_shader(const std::string &src,
                                        GLenum type) noexcept {
  GLuint shader = glCreateShader(type);
  const char *shader_c_str = src.c_str();
  const auto shader_len = static_cast<GLint>(src.size());
  glShaderSource(shader, 1, &shader_c_str, &shader_len);
  glCompileShader(shader);

  GLint compile_status = GL_TRUE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    char buf[2049];
    GLint len;
    glGetShaderInfoLog(shader, 2048, &len, buf);
    std::cerr << shader_type_to_str(type) << " " << std::string(buf, len)
              << "\n";
    glDeleteShader(shader);
  }
  return shader;
}

std::shared_ptr<renderer::shader> renderer::opengl_graphics_api::compile_shader(
    const std::string &vertex_shader_src,
    const std::string &pixel_shader_src) noexcept {
  auto vs = compile_gl_shader(vertex_shader_src, GL_VERTEX_SHADER);
  auto fs = compile_gl_shader(pixel_shader_src, GL_FRAGMENT_SHADER);
  assert(vs && fs);
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs.value());
  glAttachShader(prog, fs.value());
  glLinkProgram(prog);

  GLint stat = GL_TRUE;
  glGetProgramiv(prog, GL_LINK_STATUS, &stat);
  if (stat != GL_TRUE) {
    char buf[2049];
    GLint len;
    glGetProgramInfoLog(prog, 2048, &len, buf);
    std::cerr << std::string(buf, len) << "\n";
    glDeleteProgram(prog);
  }

  return std::make_shared<opengl_shader>(prog);
}
