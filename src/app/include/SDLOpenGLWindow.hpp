#pragma once

#include <SDL.h>

#include <string>

class SDLOpenGLWindow
{
public:
  SDLOpenGLWindow(std::string const& title, int width, int height);
  ~SDLOpenGLWindow();
  virtual void InitializeGL() = 0;
  virtual void PaintGL() = 0;
  void Show();

protected:
  void GlobalEventHandler(SDL_Event& event);
  void HandleEvent(SDL_Event event);
  virtual void OnKeyDownEvent(SDL_KeyboardEvent const& key);
  virtual void OnKeyUpEvent(SDL_KeyboardEvent const& key);
  virtual void OnMouseButtonDownEvent(SDL_MouseButtonEvent const& button);
  virtual void OnMouseButtonUpEvent(SDL_MouseButtonEvent const& button);
  virtual void OnMouseMotionEvent(SDL_MouseMotionEvent const& motion);
  virtual void OnMouseWheelEvent(SDL_MouseWheelEvent const& wheel);
  virtual void OnWindowEvent(SDL_WindowEvent const& window);
  virtual void OnProcessEvent(SDL_Event const& event);

protected:
  bool shouldClose_;
  bool shallSkipSDLEvent_;
  int width_;
  int height_;
  SDL_Window* window_;
  SDL_GLContext glContext_;
};
