#include "scene.h"

Scene::Scene()
  : width_(800)
  , height_(600)
  , quit_(false)
  , window_(nullptr, SDL_DestroyWindow)
  , shader_(nullptr){};

void
Scene::Render()
{
  shader_->SetVector3("light_color", 1.f, 1.f, 1.f);
  shader_->SetVector3("object_color", 0.f, 0.5f, 1.f);

  while (!quit_) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
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
              camera_.SetAspectRatio(width_, height_);
              break;
          }
          break;
        case SDL_QUIT:
          quit_ = true;
          break;
      }
    }

    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_->SetMatrix4("view_proj_matrix", camera_.ViewProjectionMatrix());
    shader_->SetVector3("light_src", camera_.Position());
    shader_->SetVector3("view_pos", camera_.Position());

    glDrawArrays(GL_TRIANGLES, 0, vertex_count_);

    SDL_GL_SwapWindow(window_.get());
  }
}

void
Scene::SetupOpenGL(unsigned int count, float const* data)
{
  assert(count != 0);

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
  glVertexArrayVertexBuffer(vao, 0, vbo_, 0, stride);
  glVertexArrayVertexBuffer(vao, 1, vbo_, stride, stride);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, stride);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  shader_.reset(new Shader());
  shader_->Attach(GL_VERTEX_SHADER, "shader/default.vert");
  shader_->Attach(GL_FRAGMENT_SHADER, "shader/default.frag");
  shader_->Link();
  shader_->Use();

  camera_.SetAspectRatio(AspectRatio());

  glEnable(GL_DEPTH_TEST);
}

void
Scene::KeyboardControl(Uint32 type, SDL_KeyboardEvent const& key)
{
  switch (type) {
    case SDL_KEYDOWN:
      switch (key.keysym.sym) {
        case SDLK_w:
          camera_.Moving(Camera::Translate::kUp);
          break;
        case SDLK_s:
          camera_.Moving(Camera::Translate::kDown);
          break;
        case SDLK_a:
          camera_.Moving(Camera::Translate::kLeft);
          break;
        case SDLK_d:
          camera_.Moving(Camera::Translate::kRight);
          break;
        case SDLK_RIGHT:
          camera_.Turning(Camera::Rotate::kThetaCW);
          break;
        case SDLK_LEFT:
          camera_.Turning(Camera::Rotate::kThetaCCW);
          break;
        case SDLK_UP:
          camera_.Turning(Camera::Rotate::kPhiUp);
          break;
        case SDLK_DOWN:
          camera_.Turning(Camera::Rotate::kPhiDown);
          break;
      }
      break;
    case SDL_KEYUP:
      switch (key.keysym.sym) {
        case SDLK_ESCAPE:
          quit_ = true;
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
          camera_.InitDragTranslation(button.x, button.y);
          break;
        case SDL_BUTTON_RIGHT:
          camera_.InitDragRotation(button.x, button.y);
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
  camera_.WheelZoom(-wheel.y);
}

void
Scene::MouseMotionControl(SDL_MouseMotionEvent const& motion)
{
  switch (motion.state) {
    case SDL_BUTTON_LMASK:
      camera_.DragTranslation(motion.x, motion.y);
      break;
    case SDL_BUTTON_RMASK:
      camera_.DragRotation(motion.x, motion.y);
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

  Uint32 window_flags =
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  window_.reset(SDL_CreateWindow("Isosurface",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 width_,
                                 height_,
                                 window_flags));
  SDL_SetWindowMinimumSize(window_.get(), 400, 300);

  context_ = SDL_GL_CreateContext(window_.get());
  SDL_GL_MakeCurrent(window_.get(), context_);
  gladLoadGLLoader(SDL_GL_GetProcAddress);
  SDL_GL_SetSwapInterval(1);
}

Scene::~Scene()
{
  SDL_GL_DeleteContext(context_);
  SDL_Quit();
};
