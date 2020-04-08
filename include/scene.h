#pragma once

#include <array>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

#include "SDL.h"
#include "camera.h"
#include "constants.h"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "shader.h"

class Scene
{
  int width_;
  int height_;
  int vertex_count_;
  int shader_id_;
  int gui_id_;
  int cross_section_mode_;
  bool quit_;
  GLuint vbo_;
  SDL_GLContext context_;
  std::array<float, 3> center_;
  std::array<float, 3> model_color_;
  std::array<float, 3> cross_section_point_;
  std::array<std::array<float, 3>, 3> cross_section_dir_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::vector<std::function<void(void)>> gui_routines_;
  std::vector<std::pair<std::unique_ptr<Shader>, std::function<void(void)>>> shaders_;
  Scene(Scene const&) = delete;
  Scene& operator=(Scene const&) = delete;

private:
  void DefaultShaderRoutine();
  void CrossSectionShaderRoutine();
  void ImportFonts(std::string_view dirname);
  void DefaultGui();
  void CrossSectionGui();

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
