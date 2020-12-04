#include "SDLOpenGLWindow.hpp"

#include "glad/glad.h"

SDLOpenGLWindow::SDLOpenGLWindow(std::string name, int width, int height)
  : isAlive_(true)
  , shallSkipSDLEvent_(false)
  , width_(width)
  , height_(height)
  , window_(nullptr)
  , glContext_(nullptr)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifndef NDEBUG
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, 0);
#endif

  auto const flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  window_ = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
  SDL_SetWindowMinimumSize(window_, width_, height_);
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

  while (isAlive_) {
    SDL_Event event;
    HandleEvent(event);
    PaintGL(); // Require implementation
  }
}

void
SDLOpenGLWindow::HandleEvent(SDL_Event event)
{
  while (SDL_PollEvent(&event)) {

    OnProcessEvent(event);

    if (shallSkipSDLEvent_)
      continue;

    switch (event.type) {
    case SDL_KEYDOWN:
      OnKeyDown(event.key);
      break;
    case SDL_KEYUP:
      OnKeyUp(event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
      OnMouseButtonDown(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      OnMouseButtonUp(event.button);
      break;
    case SDL_MOUSEMOTION:
      OnMouseMotion(event.motion);
      break;
    case SDL_MOUSEWHEEL:
      OnMouseWheel(event.wheel);
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        SDL_GetWindowSize(window_, &width_, &height_);
        OnWindowResized();
        break;
      }
      break;
    case SDL_QUIT:
      isAlive_ = false;
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
SDLOpenGLWindow::OnKeyDown(SDL_KeyboardEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnKeyUp(SDL_KeyboardEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseButtonDown(SDL_MouseButtonEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseButtonUp(SDL_MouseButtonEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseMotion(SDL_MouseMotionEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnMouseWheel(SDL_MouseWheelEvent const&)
{
  return; // Override this function to do things.
}

void
SDLOpenGLWindow::OnWindowResized()
{
  return; // Override this function to do things.
}
