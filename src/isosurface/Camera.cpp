#include "Camera.hpp"
#include <iostream>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_2_PI = MATH_PI * 2.0f;

Camera::Camera()
  : world_up_{0.f, 1.f, 0.f}
  , center_{0.f, 0.f, 0.f}
  , phi_(0.3f)
  , theta_(MATH_PI * 0.5f)
  , radius_(600.f)
  , aspect_ratio_(0.f)
  , view_volume_size_(200.f)
  , move_rate_(0.4f)
  , rotate_rate_(0.005f)
  , zoom_(0.8f)
  , horiz_rotate_dir(1)
{
  UpdateViewCoord();
}

Camera::~Camera() {}

glm::vec3
Camera::CartesianCoord( float phi, float theta) const
{
  using namespace std;

  float const sin_p = sinf(phi);
  float const cos_p = cosf(phi);
  float const sin_t = sinf(theta);
  float const cos_t = cosf(theta);

  return glm::vec3{sin_t * cos_p, cos_t, sin_p * sin_t};
}

void
Camera::UpdateViewCoord()
{
  using namespace glm;

  world_up_ = normalize(-radius_ * CartesianCoord(phi_, theta_ + MATH_PI * 0.25f));
  position_ = radius_ * CartesianCoord(phi_, theta_) + center_;
  forward_ = normalize(center_ - position_);
  side_ = normalize(cross(forward_, world_up_));
  up_ = cross(side_, forward_);
}

void
Camera::InitDragRotation(int x, int y)
{
  // Phi has been set between 0 and 360 degrees
  if (theta_ > MATH_PI)
    horiz_rotate_dir = -1;
  else
    horiz_rotate_dir = 1;

  rotation_origin_ = std::make_tuple(x, y, phi_, theta_);
}

void
Camera::DragRotation(int x, int y)
{
  auto const [x_o, y_o, theta_o, phi_o] = rotation_origin_;

  phi_ = NormRadian(horiz_rotate_dir * (x - x_o) * rotate_rate_ + theta_o);
  theta_ = NormRadian(-(y - y_o) * rotate_rate_ + phi_o);

  UpdateViewCoord();
}

void
Camera::InitDragTranslation(int x, int y)
{
  translation_origin_ = std::make_tuple(x, y, center_);
}

void
Camera::DragTranslation(int x, int y)
{
  auto const [x_o, y_o, target_o] = translation_origin_;

  center_ = target_o + (-(x - x_o) * side_ + (y - y_o) * up_) * move_rate_ * zoom_;

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
Camera::SetCenter(float x, float y, float z)
{
  center_ = glm::vec3{x, y, z};
  UpdateViewCoord();
}

void
Camera::SetCenter(glm::vec3 center)
{
  center_ = center;
  UpdateViewCoord();
}

void
Camera::SetPhi(float phi)
{
  theta_ = NormRadian(phi);
  UpdateViewCoord();
}

void
Camera::SetTheta(float theta)
{
  phi_ = NormRadian(theta);
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

  glm::mat4 const view = glm::lookAt(position_, center_, world_up_);

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
    center_ += -up_ * speed;
    break;
  case Translate::kDown:
    center_ += up_ * speed;
    break;
  case Translate::kRight:
    center_ += -side_ * speed;
    break;
  case Translate::kLeft:
    center_ += side_ * speed;
    break;
  case Translate::kForward:
    center_ += forward_ * speed;
    break;
  case Translate::kBackward:
    center_ += -forward_ * speed;
    break;
  }

  UpdateViewCoord();
}

void
Camera::Turning(Camera::Rotate direction)
{
  switch (direction) {
  case Rotate::kClockwise:
    phi_ = phi_ + 0.1f;
    break;
  case Rotate::kCounterClockwise:
    phi_ = phi_ - 0.1f;
    break;
  case Rotate::kPitchUp:
    theta_ = theta_ - 0.1f;
    break;
  case Rotate::kPitchDown:
    theta_ = theta_ + 0.1f;
    break;
  }

  theta_ = NormRadian(theta_);
  phi_ = NormRadian(phi_);
  UpdateViewCoord();
}

// Restrict both theta and phi to being between 0 and 360 degrees.
float
Camera::NormRadian(float radian)
{
  if (radian < 0.f)
    return radian + MATH_2_PI;
  else if (radian > MATH_2_PI)
    return radian - MATH_2_PI;
  else
    return radian;
}
