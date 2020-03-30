#pragma once

#include <functional>
#include <memory>

#include "SDL2/SDL.h"
#include "glad/glad.h"
#include "camera.h"
#include "shader.h"
#include "constants.h"

class Scene
{
  int width_;
  int height_;
  int vertex_count_;
  bool quit_;
  SDL_GLContext context_;
  Camera camera_;
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::unique_ptr<Shader> shader_;

public:
  Scene();
  ~Scene();
  void Init();
  void SetupOpenGL();
  float AspectRatio() const;
  void Render();
  void KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key);
  void MouseButtonControl(Uint32 type, SDL_MouseButtonEvent const& button);
  void MouseMotionControl(SDL_MouseMotionEvent const& motion);
  void MouseWheelControl(SDL_MouseWheelEvent const& wheel);
};
