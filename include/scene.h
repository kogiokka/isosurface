#pragma once

#include <array>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

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
  std::array<float, 3> center_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::vector<std::pair<std::unique_ptr<Shader>, std::function<void(void)>>> shaders_;
  Scene(Scene const&) = delete;
  Scene& operator=(Scene const&) = delete;

private:
  void DefaultShaderRoutine();
  void CrossSectionShaderRoutine();

public:
  Scene();
  ~Scene();
  void Init();
  void SetupOpenGL(unsigned int count, float const* data);
  void SetPosition(float x, float y, float z);
  void SetPosition(std::array<int, 3> pos);
  float AspectRatio() const;
  void Render();
  void EventHandler();
  void KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key);
  void MouseButtonControl(Uint32 type, SDL_MouseButtonEvent const& button);
  void MouseMotionControl(SDL_MouseMotionEvent const& motion);
  void MouseWheelControl(SDL_MouseWheelEvent const& wheel);
};
