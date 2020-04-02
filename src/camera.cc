#include "camera.h"

Camera::Camera()
  : world_up_{ 0.f, 1.f, 0.f }
  , target_{ 70.f, 100.f, 50.f }
  , theta_(0.3f)
  , phi_(1.4f)
  , radius_(600.f)
  , aspect_ratio_(0.f)
  , view_volume_size_(200.f)
  , move_rate_(0.8f)
  , rotate_rate_(0.01f)
  , zoom_(0.8f)
{
  UpdateViewCoord();
}

Camera::~Camera() {}

glm::vec3
Camera::CartesianCoord(float theta, float phi) const
{
  using namespace std;

  float const sin_p = sinf(phi);
  float const cos_p = cosf(phi);
  float const sin_t = sinf(theta);
  float const cos_t = cosf(theta);

  return glm::vec3{ sin_p * cos_t, cos_p, sin_p * sin_t };
}

void
Camera::UpdateViewCoord()
{
  using namespace glm;

  constexpr float degree_45 = 0.785f;
  world_up_ = normalize(-radius_ * CartesianCoord(theta_, phi_ + degree_45));

  position_ = radius_ * CartesianCoord(theta_, phi_) + target_;
  forward_ = normalize(target_ - position_);
  side_ = normalize(cross(forward_, world_up_));
  up_ = cross(side_, forward_);
}

void
Camera::InitDragRotation(int x, int y)
{
  rotation_origin_ = std::make_tuple(x, y, theta_, phi_);
}

void
Camera::DragRotation(int x, int y)
{
  auto const [x_o, y_o, theta_o, phi_o] = rotation_origin_;

  theta_ = (x - x_o) * rotate_rate_ + theta_o;
  phi_ = -(y - y_o) * rotate_rate_ + phi_o;
  if (theta_ > 6.28f)
    theta_ -= 6.28f;
  if (phi_ > 6.28f)
    phi_ -= 6.28f;

  UpdateViewCoord();
}

void
Camera::InitDragTranslation(int x, int y)
{
  translation_origin_ = std::make_tuple(x, y, target_);
}

void
Camera::DragTranslation(int x, int y)
{
  auto const [x_o, y_o, target_o] = translation_origin_;

  target_ =
    target_o + (-(x - x_o) * side_ + (y - y_o) * up_) * move_rate_ * zoom_;

  UpdateViewCoord();
}

void
Camera::SetAspectRatio(float ratio)
{
  aspect_ratio_ = ratio;
}

void
Camera::SetAspectRatio(int width, int height)
{
  aspect_ratio_ = static_cast<float>(width) / static_cast<float>(height);
}

void
Camera::SetPhi(float phi)
{
  phi_ = phi;
  UpdateViewCoord();
}

void
Camera::SetTheta(float theta)
{
  theta_ = theta;
  UpdateViewCoord();
}

void
Camera::WheelZoom(int direction)
{
  float const tmp_zoom = zoom_ + direction * 0.02;
  constexpr float min = 0.01f;
  constexpr float max = 1.0f;

  if (tmp_zoom < min) {
    zoom_ = min;
  } else if (tmp_zoom >= max) {
    zoom_ = max;
  } else {
    zoom_ = tmp_zoom;
  }
}

glm::mat4
Camera::ViewProjectionMatrix() const
{
  assert(aspect_ratio_ != 0.0);

  glm::mat4 const view = glm::lookAt(position_, target_, world_up_);

  float const v = view_volume_size_ * aspect_ratio_ * zoom_;
  float const h = view_volume_size_ * zoom_;
  return glm::ortho(-v, v, -h, h, 0.f, 1000.f) * view;
}

glm::vec3 const&
Camera::Position() const
{
  return position_;
}

glm::vec3 const&
Camera::ForwardVector() const
{
  return forward_;
}

void
Camera::Moving(Camera::Translate direction)
{
  float const speed = 10.f * move_rate_ * zoom_;
  switch (direction) {
    case Translate::kUp:
      target_ += -up_ * speed;
      break;
    case Translate::kDown:
      target_ += up_ * speed;
      break;
    case Translate::kRight:
      target_ += -side_ * speed;
      break;
    case Translate::kLeft:
      target_ += side_ * speed;
      break;
    case Translate::kForward:
      target_ += forward_ * speed;
      break;
    case Translate::kBackward:
      target_ += -forward_ * speed;
      break;
  }

  UpdateViewCoord();
}

void
Camera::Turning(Camera::Rotate direction)
{
  constexpr float max_phi = 3.10f;
  constexpr float min_phi = 0.04f;

  switch (direction) {
    case Rotate::kThetaCW:
      theta_ += 0.1f;
      break;
    case Rotate::kThetaCCW:
      theta_ -= 0.1f;
      break;
    case Rotate::kPhiUp:
      phi_ = std::max(min_phi, phi_ - 0.1f);
      break;
    case Rotate::kPhiDown:
      phi_ = std::min(max_phi, phi_ + 0.1f);
      break;
  }

  UpdateViewCoord();
}
