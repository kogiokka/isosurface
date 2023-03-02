#pragma once

#include <SDL.h>

#include <string>

class SDLOpenGLWindow
{
protected:
  bool isAlive_;
  bool shallSkipSDLEvent_;
  int width_;
  int height_;
  SDL_Window* window_;
  SDL_GLContext glContext_;
  void HandleEvent(SDL_Event event);
  virtual void OnKeyDown(SDL_KeyboardEvent const& keyEvent);
  virtual void OnKeyUp(SDL_KeyboardEvent const& keyEvent);
  virtual void OnMouseButtonDown(SDL_MouseButtonEvent const& buttonEvent);
  virtual void OnMouseButtonUp(SDL_MouseButtonEvent const& buttonEvent);
  virtual void OnMouseMotion(SDL_MouseMotionEvent const& motionEvent);
  virtual void OnMouseWheel(SDL_MouseWheelEvent const& wheelEvent);
  virtual void OnWindowResized();
  virtual void OnProcessEvent(SDL_Event const& event);

public:
  SDLOpenGLWindow(std::string name, int width, int height);
  ~SDLOpenGLWindow();
  virtual void InitializeGL() = 0;
  virtual void PaintGL() = 0;

  void Show();
};
