#pragma once

#include <string>

struct SDL_Window;

namespace renderer {
class window {
private:
  SDL_Window *SDLWin;
  friend class api_factory;

public:
  window(const std::string &title, int width, int height,
         bool fullscreen = false) noexcept;

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;
  void SetWidth(int NewWidth) noexcept;
  void SetHeight(int NewHeight) noexcept;

  bool IsFullscreen() const noexcept;
  void SetNewFullscreen(bool NewFullscreen) noexcept;

  ~window() noexcept;

protected:
  SDL_Window *GetSDLWindow() noexcept;
};
} // namespace renderer