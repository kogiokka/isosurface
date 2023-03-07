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
  void Close();
  void SetShouldProcessEvent(bool isAccepting);

protected:
  virtual void OnKeyDownEvent(SDL_KeyboardEvent const& key);
  virtual void OnKeyUpEvent(SDL_KeyboardEvent const& key);
  virtual void OnMouseButtonDownEvent(SDL_MouseButtonEvent const& button);
  virtual void OnMouseButtonUpEvent(SDL_MouseButtonEvent const& button);
  virtual void OnMouseMotionEvent(SDL_MouseMotionEvent const& motion);
  virtual void OnMouseWheelEvent(SDL_MouseWheelEvent const& wheel);
  virtual void OnWindowEvent(SDL_WindowEvent const& window);
  virtual void OnPreProcessEvent(SDL_Event const& event);

private:
  void ExecuteEventHandlers(SDL_Event& event);

protected:
  int width_;
  int height_;
  SDL_Window* window_;
  SDL_GLContext glContext_;

private:
  bool shouldClose_;
  bool shouldProcessEvent_;
};



