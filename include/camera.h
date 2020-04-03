#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cstdio>
#include <tuple>

class Camera
{
  glm::vec3 position_;
  glm::vec3 world_up_;
  glm::vec3 forward_;
  glm::vec3 side_;
  glm::vec3 up_;
  glm::vec3 target_;
  float theta_;
  float phi_;
  float radius_;
  float aspect_ratio_;
  float view_volume_size_;
  float move_rate_;
  float rotate_rate_;
  float zoom_;
  int horiz_rotate_dir;

  std::tuple<int, int, float, float> rotation_origin_;
  std::tuple<float, float, glm::vec3> translation_origin_;

  Camera(Camera const&) = delete;
  Camera& operator=(Camera const&) = delete;

public:
  enum class Translate
  {
    kUp,
    kDown,
    kLeft,
    kRight,
    kForward,
    kBackward,
  };
  enum class Rotate
  {
    kClockwise,
    kCounterClockwise,
    kPitchUp,
    kPitchDown,
  };
  Camera();
  ~Camera();
  void SetAspectRatio(float ratio);
  void SetAspectRatio(int width, int height);
  void SetTheta(float theta);
  void SetPhi(float phi);
  void UpdateViewCoord();
  void InitDragRotation(int x, int y);
  void InitDragTranslation(int x, int y);
  void DragRotation(int x, int y);
  void DragTranslation(int x, int y);
  void Moving(Camera::Translate direction);
  void Turning(Camera::Rotate direction);
  void WheelZoom(int direction);
  glm::mat4 ViewProjectionMatrix() const;
  glm::vec3 const& Position() const;
  glm::vec3 const& ForwardVector() const;

private:
  glm::vec3 CartesianCoord(float theta, float phi) const;
  inline float NormRadian(float radian);
};
