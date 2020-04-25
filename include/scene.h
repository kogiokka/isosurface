#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "SDL.h"
#include "camera.h"
#include "constants.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "model.h"
#include "shader.h"

class Scene
{
  int width_;
  int height_;
  int vertex_count_;
  int shader_id_;
  int gui_id_;
  int cross_section_mode_;
  float isovalue_;
  bool quit_;
  GLuint vao_;
  SDL_GLContext context_;
  std::filesystem::directory_entry model_dir_;
  glm::vec3 center_;
  glm::vec3 model_color_;
  glm::vec3 cross_section_point_;
  std::array<std::array<float, 3>, 3> cross_section_dir_;
  std::unordered_map<std::string, std::unique_ptr<Model>> model_list_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::vector<std::unique_ptr<Shader>> shaders_;
  std::vector<std::function<void(void)>> shader_routines_;
  std::vector<std::function<void(void)>> gui_routines_;
  Scene(Scene const&) = delete;
  Scene& operator=(Scene const&) = delete;

private:
  void ImportFonts(std::filesystem::path dir_path);
  void SelectModel(std::string const& name, bool force_regen);

public:
  Scene();
  ~Scene();
  void Init();
  void Setup();
  float AspectRatio() const;
  void Render();
  void EventHandler();
  void KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key);
  void MouseButtonControl(Uint32 type, SDL_MouseButtonEvent const& button);
  void MouseMotionControl(SDL_MouseMotionEvent const& motion);
  void MouseWheelControl(SDL_MouseWheelEvent const& wheel);
};
