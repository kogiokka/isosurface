#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
  GLuint id_;
  std::unordered_map<std::string, GLint> uniform_locations_;
  Shader(Shader const&) = delete;
  Shader& operator=(Shader const&) = delete;

public:
  Shader();
  ~Shader();
  GLuint Id() const;
  bool Attach(GLenum shader_type, std::string_view const filepath);
  void Use() const;
  void Link() const;
  int UniformLocation(std::string_view const uniform_name);
  void SetMatrix4(std::string_view const name, glm::mat4 const& mat);
  void SetVector3(std::string_view const name, glm::vec3 const& vec);
  void SetVector3(std::string_view const name, float x, float y, float z);

private:
  bool IsCompiled(GLuint const shader_object);
};
