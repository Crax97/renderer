#include "window.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>

#include <assert.h>

renderer::window::window(const std::string &title, int width, int height,
                         bool fullscreen) noexcept {

  Uint32 flags = 0;
#ifdef OPENGL_FOUND
  flags |= SDL_WINDOW_OPENGL;
#endif
#ifdef VULKAN_FOUND
  flags |= SDL_WINDOW_VULKAN;
#endif
#ifdef METAL_FOUND
  flags |= SDL_WINDOW_METAL;
#endif
  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  }

  this->SDLWin = SDL_CreateWindow(title.c_str(), 0, 0, width, height, flags);
  assert(this->SDLWin);
}

int renderer::window::GetWidth() const noexcept {
  assert(this->SDLWin);

  int w, h;
  SDL_GetWindowSize(this->SDLWin, &w, &h);
  return w;
}
int renderer::window::GetHeight() const noexcept {
  assert(this->SDLWin);

  int w, h;
  SDL_GetWindowSize(this->SDLWin, &w, &h);
  return h;
}
void renderer::window::SetWidth(int NewWidth) noexcept {
  assert(this->SDLWin);

  SDL_SetWindowSize(this->SDLWin, NewWidth, GetHeight());
}
void renderer::window::SetHeight(int NewHeight) noexcept {
  assert(this->SDLWin);

  SDL_SetWindowSize(this->SDLWin, GetWidth(), NewHeight);
}

bool renderer::window::IsFullscreen() const noexcept {
  assert(this->SDLWin);
  return SDL_GetWindowFlags(SDLWin) & SDL_WINDOW_FULLSCREEN;
}
void renderer::window::SetNewFullscreen(bool NewFullscreen) noexcept {
  assert(this->SDLWin);

  auto flags = SDL_GetWindowFlags(SDLWin);
  if (NewFullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  } else {
    flags &= ~SDL_WINDOW_FULLSCREEN;
  }
  SDL_SetWindowFullscreen(this->SDLWin, flags);
}

renderer::window::~window() noexcept {
  if (SDLWin) {
    SDL_DestroyWindow(SDLWin);
    SDLWin = nullptr;
  }
}
SDL_Window *renderer::window::GetSDLWindow() noexcept {
  assert(this->SDLWin);
  return this->SDLWin;
}