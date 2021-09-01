#pragma once

#include "Init.h"
#include "api_factory.h"
#include "graphics_api.h"
#include "window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <memory>

namespace example_support_library {
class application {
private:
  std::unique_ptr<renderer::window> m_win;
  std::unique_ptr<renderer::graphics_api> m_api;

  int m_return_code = 0;
  bool m_running = true;

  std::string m_name;
  int m_window_width;
  int m_window_height;

  void init_api() noexcept {
    m_win = std::make_unique<renderer::window>(m_name, m_window_width,
                                               m_window_height);
    m_api = renderer::api_factory::create_api("opengl", *m_win);
  }

public:
  application(std::string name, int window_width, int window_height)
      : m_name(std::move(name)), m_window_width(window_width),
        m_window_height(window_height) {}

  int loop() {

    renderer::Init();
    init_api();
    on_app_startup();

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    float delta_time = 0.0f;

    while (m_running) {
      last = now;
      SDL_Event evt;
      SDL_PollEvent(&evt);
      switch (evt.type) {
      case SDL_QUIT:
        app_exit(0);
        break;
      }
      on_app_loop(delta_time);
      now = SDL_GetPerformanceCounter();
      delta_time = static_cast<float>(now - last) /
                   static_cast<float>(SDL_GetPerformanceFrequency());
    }

    on_app_shutdown();
    renderer::Quit();
    return m_return_code;
  }

  std::unique_ptr<renderer::graphics_api> &get_graphics_api() noexcept {
    return m_api;
  }

  void app_exit(int return_code) {
    m_running = false;
    m_return_code = return_code;
  }

  virtual void on_app_startup() {}
  virtual void on_app_loop(float delta_seconds) {}
  virtual void on_app_shutdown() {}
};
} // namespace example_support_library