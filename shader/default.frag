#version 450

in vec3 attr_pos;
in vec3 attr_normal;
in vec3 attr_color;

out vec4 color;

uniform vec3 light_src;
uniform vec3 light_color;
uniform vec3 view_pos;

void main()
{

  vec3 norm = normalize(attr_normal);
  vec3 light_dir = normalize(light_src - attr_pos);

  float ambient_I = 0.1;
  float diffusion_I = max(dot(norm, light_dir), 0);
  float specular_coef = 0.5;

  vec3 ambient = ambient_I * light_color;
  vec3 diffusion = diffusion_I * light_color;
  vec3 view_dir = normalize(view_pos - attr_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float specular_I = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = specular_coef * specular_I * light_color;

  color = vec4((ambient + diffusion + specular) * attr_color, 1.0);
}