#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "SDLOpenGLWindow.hpp"
#include "Shader.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

class MainWindow : public SDLOpenGLWindow
{
  int vertex_count_;
  int shader_id_;
  int gui_id_;
  int cross_section_mode_;
  float isovalue_;
  GLuint vao_;
  std::filesystem::directory_entry model_dir_;
  glm::vec3 center_;
  glm::vec3 model_color_;
  glm::vec3 cross_section_point_;
  std::array<std::array<float, 3>, 3> cross_section_dir_;
  std::unordered_map<std::string, std::unique_ptr<Model>> model_list_;
  std::unique_ptr<Camera> camera_;
  std::vector<std::unique_ptr<Shader>> shaders_;
  std::vector<std::function<void(void)>> shader_routines_;
  std::vector<std::function<void(void)>> gui_routines_;
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

private:
  void ImportFonts(std::filesystem::path dir_path);
  void GenIsosurface(std::string const& name, bool force_regen, int method);

private:
  virtual void InitializeGL() override;
  virtual void PaintGL() override;
  virtual void OnProcessEvent(SDL_Event const& event) override;
  virtual void OnWindowResized() override;
  virtual void OnMouseButtonDown(SDL_MouseButtonEvent const& buttonDown) override;
  virtual void OnMouseWheel(SDL_MouseWheelEvent const& wheel) override;
  virtual void OnMouseMotion(SDL_MouseMotionEvent const& motion) override;
  virtual void OnKeyDown(SDL_KeyboardEvent const& keydown) override;

public:
  MainWindow(std::string const& name, int width, int height);
  ~MainWindow();
};
