#include <cstdlib>
#include <iostream>
#include <string>

#include <SDL.h>
#include <glad/glad.h>

#include "SDLOpenGLWindow.hpp"

SDLOpenGLWindow::SDLOpenGLWindow(std::string const& title, int width, int height)
  : shouldClose_(false)
  , shallSkipSDLEvent_(false)
  , width_(width)
  , height_(height)
  , window_(nullptr)
  , glContext_(nullptr)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

#ifdef __linux__
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif

  const Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
  if (window_ == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
  }

  SDL_SetWindowMinimumSize(window_, 400, 300);
  glContext_ = SDL_GL_CreateContext(window_);
  SDL_GL_MakeCurrent(window_, glContext_);
  SDL_GL_SetSwapInterval(1);

  gladLoadGLLoader(SDL_GL_GetProcAddress);
}

SDLOpenGLWindow::~SDLOpenGLWindow()
{
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

void
SDLOpenGLWindow::Show()
{
  InitializeGL(); // Require implementation

  while (!shouldClose_) {
    SDL_Event event;
    HandleEvent(event);
    PaintGL(); // Require implementation
  }
}

void
SDLOpenGLWindow::HandleEvent(SDL_Event event)
{
  while (SDL_PollEvent(&event)) {

    switch (event.type) {
    case SDL_QUIT:
      shouldClose_ = true;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_q) {
        if (event.key.keysym.mod & KMOD_CTRL) {
          shouldClose_ = true;
        }
      }
      break;
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        SDL_GetWindowSize(window_, &width_, &height_);
      }
      break;
    }

    OnProcessEvent(event);

    if (shallSkipSDLEvent_)
      continue;

    switch (event.type) {
    case SDL_KEYDOWN:
      OnKeyDownEvent(event.key);
      break;
    case SDL_KEYUP:
      OnKeyUpEvent(event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
      OnMouseButtonDownEvent(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      OnMouseButtonUpEvent(event.button);
      break;
    case SDL_MOUSEMOTION:
      OnMouseMotionEvent(event.motion);
      break;
    case SDL_MOUSEWHEEL:
      OnMouseWheelEvent(event.wheel);
      break;
    case SDL_WINDOWEVENT:
      OnWindowEvent(event.window);
      break;
    case SDL_QUIT:
      shouldClose_ = true;
      break;
    }
  }
}

void
SDLOpenGLWindow::OnProcessEvent(SDL_Event const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnKeyDownEvent(SDL_KeyboardEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnKeyUpEvent(SDL_KeyboardEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseButtonDownEvent(SDL_MouseButtonEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseButtonUpEvent(SDL_MouseButtonEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseMotionEvent(SDL_MouseMotionEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseWheelEvent(SDL_MouseWheelEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnWindowEvent(SDL_WindowEvent const&)
{
  return; // Override this function to do things.
}
