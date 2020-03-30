#include "camera.h"

Camera::Camera()
  : world_up_{ 0.f, 1.f, 0.f }
  , target_{ 0.f, 0.f, 0.f }
  , theta_(0.3f)
  , phi_(1.4f)
  , radius_(5.f)
  , aspect_ratio_(0.f)
  , view_volume_size_(20.f)
  , move_rate_(0.01f)
  , rotate_rate_(0.01f)
  , zoom_(0.2f)
{
  UpdateViewCoord();
}

Camera::~Camera() {}

void
Camera::UpdateViewCoord()
{
  using namespace glm;

  float x = sin(phi_) * sin(theta_);
  float y = cos(phi_);
  float z = sin(phi_) * cos(theta_);
  position_ = radius_ * vec3{ x, y, z } + target_;
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
  constexpr float max_phi = 3.10f;
  constexpr float min_phi = 0.04f;
  int const x_o = std::get<0>(rotation_origin_);
  int const y_o = std::get<1>(rotation_origin_);
  float const theta_o = std::get<2>(rotation_origin_);
  float const phi_o = std::get<3>(rotation_origin_);

  theta_ = -(x - x_o) * rotate_rate_ + theta_o;
  float const tmp_phi = -(y - y_o) * rotate_rate_ + phi_o;
  if (tmp_phi >= max_phi) {
    phi_ = max_phi;
  } else if (tmp_phi <= min_phi) {
    phi_ = min_phi;
  } else {
    phi_ = tmp_phi;
  }

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
  float const x_o = static_cast<float>(std::get<0>(translation_origin_));
  float const y_o = static_cast<float>(std::get<1>(translation_origin_));
  glm::vec3 const target_o = std::get<2>(translation_origin_);

  target_ = (side_ * -(x - x_o) + up_ * (y - y_o)) * move_rate_ + target_o;

  UpdateViewCoord();
}

void
Camera::Moving(Camera::Translate direction)
{
  switch (direction) {
    case Translate::kUp:
      target_ += -up_;
      break;
    case Translate::kDown:
      target_ += up_;
      break;
    case Translate::kRight:
      target_ += side_;
      break;
    case Translate::kLeft:
      target_ += -side_;
      break;
    case Translate::kForward:
      target_ += forward_;
      break;
    case Translate::kBackward:
      target_ += -forward_;
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
  constexpr float min = 0.1f;
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
  return glm::ortho(-v, v, -h, h, 0.f, 100.f) * view;
}

glm::vec3 const&
Camera::Position() const
{
  return position_;
}