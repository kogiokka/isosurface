#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 view_proj_matrix;

out vec3 attr_pos;
out vec3 attr_normal;

void main()
{
  gl_Position = view_proj_matrix * vec4(position, 1.0);
  attr_pos = position;
  attr_normal = normal;
}
