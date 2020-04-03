#include "camera.h"

Camera::Camera()
  : world_up_{ 0.f, 1.f, 0.f }
  , center_{ 0.f, 0.f, 0.f }
  , theta_(0.3f)
  , phi_(1.4f)
  , radius_(600.f)
  , aspect_ratio_(0.f)
  , view_volume_size_(200.f)
  , move_rate_(0.4f)
  , rotate_rate_(0.01f)
  , zoom_(0.8f)
  , horiz_rotate_dir(1)
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

  position_ = radius_ * CartesianCoord(theta_, phi_) + center_;
  forward_ = normalize(center_ - position_);
  side_ = normalize(cross(forward_, world_up_));
  up_ = cross(side_, forward_);
}

void
Camera::InitDragRotation(int x, int y)
{
  constexpr float half = 3.1415926f;

  // Phi has been set between 0 and 360 degrees
  if (phi_ > half)
    horiz_rotate_dir = -1;
  else
    horiz_rotate_dir = 1;

  rotation_origin_ = std::make_tuple(x, y, theta_, phi_);
}

void
Camera::DragRotation(int x, int y)
{
  auto const [x_o, y_o, theta_o, phi_o] = rotation_origin_;

  theta_ = NormRadian(horiz_rotate_dir * (x - x_o) * rotate_rate_ + theta_o);
  phi_ = NormRadian(-(y - y_o) * rotate_rate_ + phi_o);

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
  center_ = glm::vec3{ x, y, z };
}

void
Camera::SetPhi(float phi)
{
  phi_ = NormRadian(phi);
  UpdateViewCoord();
}

void
Camera::SetTheta(float theta)
{
  theta_ = NormRadian(theta);
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
    theta_ = theta_ + 0.1f;
    break;
  case Rotate::kCounterClockwise:
    theta_ = theta_ - 0.1f;
    break;
  case Rotate::kPitchUp:
    phi_ = phi_ - 0.1f;
    break;
  case Rotate::kPitchDown:
    phi_ = phi_ + 0.1f;
    break;
  }

  phi_ = NormRadian(phi_);
  theta_ = NormRadian(theta_);
  UpdateViewCoord();
}

// Restrict both theta and phi to being between 0 and 360 degrees.
float
Camera::NormRadian(float radian)
{
  constexpr float full = 6.2831853f;

  if (radian < 0.f)
    return radian + full;
  else if (radian > full)
    return radian - full;
  else
    return radian;
}
