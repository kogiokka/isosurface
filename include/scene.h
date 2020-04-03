#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <tuple>

#include "SDL2/SDL.h"
#include "camera.h"
#include "constants.h"
#include "glad/glad.h"
#include "shader.h"

class Scene
{
  int width_;
  int height_;
  int vertex_count_;
  unsigned int shader_id_;
  bool quit_;
  GLuint vbo_;
  SDL_GLContext context_;
  std::vector<std::pair<std::unique_ptr<Shader>, std::function<void(void)>>> shaders_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;

private:
  void DefaultShaderRoutine();
  void CrossSectionShaderRoutine();

public:
  Scene();
  ~Scene();
  void Init();
  void SetupOpenGL(unsigned int count, float const* data);
  float AspectRatio() const;
  void Render();
  void EventHandler();
  void KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key);
  void MouseButtonControl(Uint32 type, SDL_MouseButtonEvent const& button);
  void MouseMotionControl(SDL_MouseMotionEvent const& motion);
  void MouseWheelControl(SDL_MouseWheelEvent const& wheel);
};
