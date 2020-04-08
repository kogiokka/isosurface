#include "scene.h"

Scene::Scene()
  : width_(800)
  , height_(600)
  , vertex_count_(0)
  , shader_id_(0)
  , gui_id_(0)
  , cross_section_mode_(0)
  , quit_(false)
  , vbo_(0)
  , context_(nullptr)
  , center_{ 0.f, 0.f, 0.f }
  , model_color_{ 0.f, 0.5f, 1.f }
  , cross_section_point_{ 0.f, 0.f, 0.f }
  , cross_section_dir_{ { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } } }
  , camera_(std::make_unique<Camera>())
  , window_(nullptr, SDL_DestroyWindow)
  , shaders_(0){};

void
Scene::DefaultShaderRoutine()
{
  auto& s = shaders_[shader_id_].first;
  s->SetVector3("light_color", 1.f, 1.f, 1.f);
  s->SetVector3("model_color", model_color_);
  s->SetMatrix4("view_proj_matrix", camera_->ViewProjectionMatrix());
  s->SetVector3("light_src", camera_->Position());
  s->SetVector3("view_pos", camera_->Position());
}

void
Scene::CrossSectionShaderRoutine()
{
  auto& s = shaders_[shader_id_].first;
  s->SetVector3("light_color", 1.f, 1.f, 1.f);
  s->SetVector3("light_src", camera_->Position());
  s->SetVector3("model_color", model_color_);
  s->SetVector3("cross_section.point", cross_section_point_);
  s->SetVector3("cross_section.normal", cross_section_dir_.at(cross_section_mode_));
  s->SetMatrix4("view_proj_matrix", camera_->ViewProjectionMatrix());
  s->SetVector3("view_pos", camera_->Position());
}

void
Scene::Render()
{
  while (!quit_) {
    EventHandler();
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders_[shader_id_].second();
    gui_routines_[gui_id_]();

    glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window_.get());
  }
}

void
Scene::SetupOpenGL(unsigned int count, float const* data)
{
  assert(count != 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplSDL2_InitForOpenGL(window_.get(), context_);
  ImportFonts("res/fonts");

  vertex_count_ = count;
  GLuint vao;
  unsigned int stride = 3 * sizeof(float);

  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo_);

  glBindVertexArray(vao);
  glBindBuffer(GL_VERTEX_ARRAY, vbo_);

  glNamedBufferStorage(vbo_, 2 * count * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(vbo_, 0, 2 * count * sizeof(float), data);

  // Interleaved data
  glVertexArrayVertexBuffer(vao, 0, vbo_, 0, stride * 2);
  glVertexArrayVertexBuffer(vao, 1, vbo_, stride, stride * 2);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  shaders_.emplace_back(std::make_unique<Shader>(), std::bind(&Scene::DefaultShaderRoutine, this));
  shaders_.emplace_back(std::make_unique<Shader>(), std::bind(&Scene::CrossSectionShaderRoutine, this));
  shaders_[0].first->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shaders_[0].first->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shaders_[1].first->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shaders_[1].first->Attach(GL_FRAGMENT_SHADER, "shader/cross_section.frag");

  gui_routines_.emplace_back(std::bind(&Scene::DefaultGui, this));
  gui_routines_.emplace_back(std::bind(&Scene::CrossSectionGui, this));

  for (auto const& s : shaders_)
    s.first->Link();

  shaders_[shader_id_].first->Use();
  camera_->SetAspectRatio(AspectRatio());
  camera_->SetCenter(center_);

  glEnable(GL_DEPTH_TEST);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void
Scene::DefaultGui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window_.get());
  ImGui::NewFrame();
  ImGui::Begin("Model");
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Mode")) {
      if (ImGui::MenuItem("Normal")) {
        shader_id_ = 0;
        gui_id_ = 0;
        shaders_[shader_id_].first->Use();
      }
      if (ImGui::MenuItem("Cross Section")) {
        shader_id_ = 1;
        gui_id_ = 1;
        shaders_[shader_id_].first->Use();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  ImGui::ColorEdit3("Color", model_color_.data());
  ImGui::End();
}

void
Scene::CrossSectionGui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window_.get());
  ImGui::NewFrame();

  ImGui::Begin("Model");
  ImGui::ColorEdit3("Color", model_color_.data());
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Mode")) {
      if (ImGui::MenuItem("Normal")) {
        shader_id_ = 0;
        gui_id_ = 0;
        shaders_[shader_id_].first->Use();
      }
      if (ImGui::MenuItem("Cross Section")) {
        shader_id_ = 1;
        gui_id_ = 1;
        shaders_[shader_id_].first->Use();
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
  ImGui::SliderFloat("", &cross_section_point_[cross_section_mode_], 0.0f, center_[cross_section_mode_] * 2.0f, "%.2f");
  ImGui::End();
}

void
Scene::ImportFonts(std::string_view dirname)
{
  namespace fs = std::filesystem;

  if (!fs::exists(dirname) || !fs::is_directory(dirname))
    return;

  ImGuiIO& io = ImGui::GetIO();
  for (fs::directory_entry const& entry : fs::recursive_directory_iterator(dirname)) {
    if (!entry.is_regular_file())
      continue;
    std::string ext = entry.path().extension().string();
    if (ext != ".ttf" && ext != ".otf")
      continue;
    io.Fonts->AddFontFromFileTTF(entry.path().c_str(), 18);
  }
}

void
Scene::EventHandler()
{
  SDL_Event event;
  ImGuiIO& io = ImGui::GetIO();
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
      continue;
    switch (event.type) {
    case SDL_MOUSEMOTION:
      MouseMotionControl(event.motion);
      break;
    case SDL_MOUSEBUTTONDOWN:
      MouseButtonControl(event.type, event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      MouseButtonControl(event.type, event.button);
      break;
    case SDL_KEYDOWN:
      KeyboardControl(event.type, event.key);
      break;
    case SDL_KEYUP:
      KeyboardControl(event.type, event.key);
      break;
    case SDL_MOUSEWHEEL:
      MouseWheelControl(event.wheel);
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        SDL_GetWindowSize(window_.get(), &width_, &height_);
        camera_->SetAspectRatio(width_, height_);
        break;
      }
      break;
    case SDL_QUIT:
      quit_ = true;
      break;
    }
  }
}

void
Scene::KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key)
{
  switch (type) {
  case SDL_KEYDOWN:
    switch (key.keysym.sym) {
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
      camera_.reset(new Camera());
      camera_->SetAspectRatio(AspectRatio());
      camera_->SetCenter(center_[0], center_[1], center_[2]);
      break;
    case SDLK_q:
      if (key.keysym.mod & KMOD_CTRL)
        quit_ = true;
      break;
    }
    break;
  case SDL_KEYUP:
    switch (key.keysym.sym) {
    case SDLK_ESCAPE:
      break;
    }
    break;
  }
}

void
Scene::MouseButtonControl(Uint32 type, const SDL_MouseButtonEvent& button)
{
  switch (type) {
  case SDL_MOUSEBUTTONDOWN:
    switch (button.button) {
    case SDL_BUTTON_LEFT:
      camera_->InitDragTranslation(button.x, button.y);
      break;
    case SDL_BUTTON_RIGHT:
      camera_->InitDragRotation(button.x, button.y);
      break;
    }
    break;
  case SDL_MOUSEBUTTONUP:
    switch (button.button) {
    case SDL_BUTTON_LEFT:
      break;
    case SDL_BUTTON_RIGHT:
      break;
    }
    break;
  }
}

void
Scene::MouseWheelControl(const SDL_MouseWheelEvent& wheel)
{
  camera_->WheelZoom(-wheel.y);
}

void
Scene::MouseMotionControl(SDL_MouseMotionEvent const& motion)
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

float
Scene::AspectRatio() const
{
  return static_cast<float>(width_) / static_cast<float>(height_);
};

void
Scene::Init()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  window_.reset(
    SDL_CreateWindow("Isosurface", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, window_flags));
  SDL_SetWindowMinimumSize(window_.get(), 400, 300);

  context_ = SDL_GL_CreateContext(window_.get());
  SDL_GL_MakeCurrent(window_.get(), context_);
  gladLoadGLLoader(SDL_GL_GetProcAddress);
  SDL_GL_SetSwapInterval(1);
}

void
Scene::SetPosition(float x, float y, float z)
{
  center_ = { x, y, z };
}

void
Scene::SetPosition(std::array<int, 3> pos)
{
  std::copy(begin(pos), end(pos), begin(center_));
}

Scene::~Scene()
{
  SDL_GL_DeleteContext(context_);
  SDL_Quit();
};
