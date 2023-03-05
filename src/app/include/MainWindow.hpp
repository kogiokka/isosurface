#pragma once

#include "Camera.hpp"
#include "SDLOpenGLWindow.hpp"
#include "Shader.hpp"
#include "Model.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <filesystem>
#include <vector>

class MainWindow : public SDLOpenGLWindow
{
  int vertexCount_;
  int shaderId_;
  int guiMode_;
  int crossSectionDirection_;
  float isovalue_;
  GLuint vao_;
  GLuint vbo_;
  std::string importPath_;
  glm::vec3 center_;
  glm::vec3 model_color_;
  glm::vec3 crossSectionPos_;
  std::array<std::array<float, 3>, 3> cross_section_dir_;
  std::unique_ptr<Model> model_;
  std::unique_ptr<Camera> camera_;
  std::vector<std::unique_ptr<Shader>> shaders_;
  std::vector<std::filesystem::path> modelFiles_;
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

private:
  void ImportVolumeDataFiles(std::string const& path, bool recursive = false);
  void ImportFonts(std::string const& directory);
  void GenIsosurface(std::string const& infFile, int method);

private:
  virtual void InitializeGL() override;
  virtual void PaintGL() override;
  virtual void OnMouseButtonDownEvent(SDL_MouseButtonEvent const& buttonDown) override;
  virtual void OnMouseWheelEvent(SDL_MouseWheelEvent const& wheel) override;
  virtual void OnMouseMotionEvent(SDL_MouseMotionEvent const& motion) override;
  virtual void OnKeyDownEvent(SDL_KeyboardEvent const& keydown) override;
  virtual void OnWindowEvent(SDL_WindowEvent const& window) override;
  virtual void OnPreProcessEvent(SDL_Event const& event) override;

public:
  MainWindow(std::string const& name, int width, int height);
  ~MainWindow();
};
