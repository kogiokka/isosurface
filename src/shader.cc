#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
  : id_(glCreateProgram())
{
  assert(id_ != 0);
}

Shader::~Shader()
{
  glDeleteProgram(id_);
}

unsigned int
Shader::Id() const
{
  return static_cast<unsigned int>(id_);
}

bool
Shader::Attach(GLenum shader_type, std::string_view const filepath)
{
  using namespace std;

  ifstream file;
  string source;

  file.open(filepath.data());
  if (file.fail())
    return false;
  source.resize(filesystem::file_size(filepath));
  file.read(source.data(), source.size());
  file.close();

  char const* const shader_source_array[1] = { source.c_str() };

  GLuint shader_object = glCreateShader(shader_type);
  glShaderSource(shader_object, 1, shader_source_array, nullptr);
  glCompileShader(shader_object);
  if (!IsCompiled(shader_object))
    return false;
  glAttachShader(id_, shader_object);
  glDeleteShader(shader_object);

  return true;
}

int
Shader::UniformLocation(std::string_view const uniform_name)
{
  char const* name = uniform_name.data();

  if (uniform_locations_.find(name) != uniform_locations_.end()) {
    return uniform_locations_[name];
  }

  int location = glGetUniformLocation(id_, name);
  if (location == -1) {
    fprintf(stderr, "Uniform %s does not exist.\n", name);
  }
  uniform_locations_[name] = location;
  return location;
}

void
Shader::SetMatrix4(std::string_view const name, glm::mat4 const& mat)
{
  glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void
Shader::SetVector3(const std::string_view name, float x, float y, float z)
{
  glUniform3f(UniformLocation(name), x, y, z);
};

void
Shader::SetVector3(const std::string_view name, const glm::vec3& vec)
{
  glUniform3fv(UniformLocation(name), 1, glm::value_ptr(vec));
}

void
Shader::Use() const
{
  glUseProgram(id_);
}

bool
Shader::Link() const
{
  glLinkProgram(id_);

  GLint success;
  glGetProgramiv(id_, GL_LINK_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint log_length;
  std::string log;
  glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &log_length);
  log.resize(log_length);
  glGetProgramInfoLog(id_, log_length, nullptr, log.data());
  glDeleteProgram(id_);

  fprintf(stderr, "%s\n", log.data());
  return false;
}

bool
Shader::IsCompiled(GLuint const shader_object)
{
  GLint success;
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint log_length;
  std::string log;

  glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &log_length);
  log.resize(log_length);
  glGetShaderInfoLog(shader_object, log_length, nullptr, log.data());
  glDeleteShader(shader_object);

  fprintf(stderr, "%s\n", log.data());

  return false;
}
