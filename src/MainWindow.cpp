#include "MainWindow.hpp"

#include <cstddef>

using std::size_t;
namespace fs = std::filesystem;

MainWindow::MainWindow(std::string const& name, int width, int height)
  : SDLOpenGLWindow(name, width, height)
  , vertexCount_(0)
  , shaderId_(0)
  , guiMode_(0)
  , cross_section_mode_(0)
  , isovalue_(80.f)
  , vao_(0)
  , model_dir_("models/default")
  , center_{0.f, 0.f, 0.f}
  , model_color_{0.f, 0.5f, 1.f}
  , cross_section_point_{0.f, 0.f, 0.f}
  , cross_section_dir_{{{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}}}
  , model_(nullptr)
  , camera_(std::make_unique<Camera>())
  , shaders_(0)
{
}

MainWindow::~MainWindow() {}

void
MainWindow::InitializeGL()
{
#ifndef NDEBUG
  glDebugMessageCallback(
    []([[maybe_unused]] GLenum source,
       [[maybe_unused]] GLenum type,
       [[maybe_unused]] GLuint id,
       [[maybe_unused]] GLenum severity,
       [[maybe_unused]] GLsizei length,
       [[maybe_unused]] GLchar const* message,
       [[maybe_unused]] void const* user_param) noexcept {
      fprintf(stderr, "TYPE:%x, Severity:%x MSG: %s.\n", type, severity, message);
    },
    nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(window_, glContext_);
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 1.0f;
  ImportFonts("res/fonts");

  for (fs::path const& entry : fs::recursive_directory_iterator(model_dir_)) {
    if (entry.extension().string() == ".raw")
      modelNames_.push_back(entry.stem());
  }
  std::sort(modelNames_.begin(), modelNames_.end());

  glCreateVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  shaders_.emplace_back(std::make_unique<Shader>());
  shaders_[0]->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shaders_[0]->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shaders_.emplace_back(std::make_unique<Shader>());
  shaders_[1]->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shaders_[1]->Attach(GL_FRAGMENT_SHADER, "shader/cross_section.frag");

  for (auto const& shader : shaders_)
    shader->Link();

  shaders_[shaderId_]->Use();
  camera_->SetAspectRatio(width_, height_);

  glEnable(GL_DEPTH_TEST);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void
MainWindow::PaintGL()
{
  glViewport(0, 0, width_, height_);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window_);
  ImGui::NewFrame();

  switch (guiMode_) {
  case 0: {
    ImGui::Begin("Model");
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Mode")) {
        if (ImGui::MenuItem("Normal")) {
          shaderId_ = 0;
          guiMode_ = 0;
          shaders_[shaderId_]->Use();
        }
        if (ImGui::MenuItem("Cross Section")) {
          shaderId_ = 1;
          guiMode_ = 1;
          shaders_[shaderId_]->Use();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    static size_t curr = 0;
    static int method = 0;

    if (ImGui::BeginCombo("Select Model", modelNames_[curr].data())) {
      for (size_t i = 0; i < modelNames_.size(); ++i) {
        if (ImGui::Selectable(modelNames_[i].c_str(), i == curr)) {
          curr = i;
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::RadioButton("Marching Cube", method == 0)) {
      method = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Marching Tetrahera", method == 1)) {
      method = 1;
    }
    ImGui::SliderFloat("Isovalue", &isovalue_, 0.f, 2000.f);
    if (ImGui::Button("Generate")) {
      GenIsosurface(modelNames_.at(curr), method);
    }
    ImGui::ColorEdit3("Color", glm::value_ptr(model_color_));
    ImGui::End();
  } break;
  case 1: {
    ImGui::Begin("Model");
    ImGui::ColorEdit3("Color", glm::value_ptr(model_color_));
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("Mode")) {
        if (ImGui::MenuItem("Normal")) {
          shaderId_ = 0;
          guiMode_ = 0;
          shaders_[shaderId_]->Use();
        }
        if (ImGui::MenuItem("Cross Section")) {
          shaderId_ = 1;
          guiMode_ = 1;
          shaders_[shaderId_]->Use();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
    if (ImGui::RadioButton("X", cross_section_mode_ == 0)) {
      cross_section_mode_ = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Y", cross_section_mode_ == 1)) {
      cross_section_mode_ = 1;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Z", cross_section_mode_ == 2)) {
      cross_section_mode_ = 2;
    }
    ImGui::SliderFloat(
      "", &cross_section_point_[cross_section_mode_], 0.0f, center_[cross_section_mode_] * 2.0f, "%.2f");
    ImGui::End();
  } break;
  }

  // Run shader routine after GenModelIsosurface updating the camera.
  switch (shaderId_) {
  case 0: {
    auto& s = shaders_[shaderId_];
    s->SetVector3("light_color", 1.f, 1.f, 1.f);
    s->SetVector3("model_color", model_color_);
    s->SetMatrix4("view_proj_matrix", camera_->ViewProjectionMatrix());
    s->SetVector3("light_src", camera_->Position());
    s->SetVector3("view_pos", camera_->Position());
  } break;
  case 1: {
    auto& s = shaders_[shaderId_];
    s->SetVector3("light_color", 1.f, 1.f, 1.f);
    s->SetVector3("light_src", camera_->Position());
    s->SetVector3("model_color", model_color_);
    s->SetVector3("cross_section.point", cross_section_point_);
    s->SetVector3("cross_section.normal", cross_section_dir_.at(cross_section_mode_));
    s->SetMatrix4("view_proj_matrix", camera_->ViewProjectionMatrix());
    s->SetVector3("view_pos", camera_->Position());
  } break;
  }

  glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(window_);
}

void
MainWindow::GenIsosurface(std::string const& name, int method)
{
  if (model_ != nullptr) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &model_->Id());
  }

  fs::path infPath = model_dir_.path() / (name + ".inf");
  fs::path rawPath = model_dir_.path() / (name + ".raw");
  model_ = std::make_unique<Model>(infPath, rawPath);
  model_->GenIsosurface(isovalue_, method);
  vertexCount_ = model_->VertexCount();

  glCreateBuffers(1, &model_->Id());
  glNamedBufferStorage(model_->Id(), 2 * vertexCount_ * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(model_->Id(), 0, 2 * vertexCount_ * sizeof(float), model_->RenderData());
  center_ = model_->Center();
  camera_ = std::make_unique<Camera>();
  camera_->SetAspectRatio(width_, height_);
  camera_->SetCenter(center_);

  unsigned int stride = 3 * sizeof(float);
  // Interleaved data
  glVertexArrayVertexBuffer(vao_, 0, model_->Id(), 0, stride * 2);
  glVertexArrayVertexBuffer(vao_, 1, model_->Id(), stride, stride * 2);
}

void
MainWindow::ImportFonts(std::filesystem::path dir_path)
{
  if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
    return;

  ImGuiIO& io = ImGui::GetIO();
  for (fs::directory_entry const& entry : fs::recursive_directory_iterator(dir_path)) {
    if (!entry.is_regular_file())
      continue;
    auto const& ext = entry.path().extension().string();
    if (ext != ".ttf" && ext != ".otf")
      continue;
    io.Fonts->AddFontFromFileTTF(entry.path().c_str(), 18);
  }
}

void
MainWindow::OnProcessEvent(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);
  auto const& io = ImGui::GetIO();
  if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    shallSkipSDLEvent_ = true;
    return;
  }
  shallSkipSDLEvent_ = false;
}

void
MainWindow::OnWindowResized()
{
  camera_->SetAspectRatio(width_, height_);
}

void
MainWindow::OnKeyDown(SDL_KeyboardEvent const& keydown)
{
  switch (keydown.keysym.sym) {
  case SDLK_w:
    camera_->Moving(Camera::Translate::kUp);
    break;
  case SDLK_s:
    camera_->Moving(Camera::Translate::kDown);
    break;
  case SDLK_a:
    camera_->Moving(Camera::Translate::kLeft);
    break;
  case SDLK_d:
    camera_->Moving(Camera::Translate::kRight);
    break;
  case SDLK_RIGHT:
    camera_->Turning(Camera::Rotate::kClockwise);
    break;
  case SDLK_LEFT:
    camera_->Turning(Camera::Rotate::kCounterClockwise);
    break;
  case SDLK_UP:
    camera_->Turning(Camera::Rotate::kPitchDown);
    break;
  case SDLK_DOWN:
    camera_->Turning(Camera::Rotate::kPitchUp);
    break;
  case SDLK_r:
    camera_ = std::make_unique<Camera>();
    camera_->SetAspectRatio(width_, height_);
    camera_->SetCenter(center_);
    break;
  case SDLK_q:
    if (keydown.keysym.mod & KMOD_CTRL) {
      isAlive_ = false;
    }
    break;
  }
}

void
MainWindow::OnMouseButtonDown(SDL_MouseButtonEvent const& buttonDown)
{
  switch (buttonDown.button) {
  case SDL_BUTTON_LEFT:
    camera_->InitDragTranslation(buttonDown.x, buttonDown.y);
    break;
  case SDL_BUTTON_RIGHT:
    camera_->InitDragRotation(buttonDown.x, buttonDown.y);
    break;
  }
}

void
MainWindow::OnMouseWheel(SDL_MouseWheelEvent const& wheel)
{
  camera_->WheelZoom(-wheel.y);
}

void
MainWindow::OnMouseMotion(SDL_MouseMotionEvent const& motion)
{
  switch (motion.state) {
  case SDL_BUTTON_LMASK:
    camera_->DragTranslation(motion.x, motion.y);
    break;
  case SDL_BUTTON_RMASK:
    camera_->DragRotation(motion.x, motion.y);
    break;
  }
}
