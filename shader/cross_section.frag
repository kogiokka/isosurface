#version 450

in vec3 attr_pos;
in vec3 attr_normal;

out vec4 color;

uniform vec3 light_src;
uniform vec3 light_color;
uniform vec3 view_pos;
uniform vec3 object_color;

struct CrossSection
{
  vec3 normal;
  vec3 point;
};

uniform CrossSection cross_section;

bool IsCutOff()
{
  float test_result = dot(normalize(cross_section.point - attr_pos), cross_section.normal);

  if (test_result < 0.0)
    return false;
  else
    return true;
}

void main()
{
  if (IsCutOff()) {
    discard;
  }
  vec3 norm = normalize(attr_normal);
  vec3 light_dir = normalize(light_src - attr_pos);

  const float ambient_I = 0.1;
  float diffusion_I = dot(norm, light_dir);
  if (diffusion_I < 0.0) {
    diffusion_I = -diffusion_I;
    norm = -norm;
  }
  float specular_coef = 0.5;

  vec3 ambient = ambient_I * light_color;
  vec3 diffusion = diffusion_I * light_color;
  vec3 view_dir = normalize(view_pos - attr_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float specular_I = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = specular_coef * specular_I * light_color;

  color = vec4((ambient + diffusion + specular) * object_color, 1.0);
}
