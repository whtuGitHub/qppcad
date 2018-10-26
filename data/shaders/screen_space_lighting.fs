R"(  
#version 330
uniform vec3 v_color;
uniform float f_specular_intensity;
uniform float f_specular_alpha;
in vec3 fs_normal;
in vec3 fs_position;
out vec4 color;

void main(void){
  vec3 sp_light_pos = normalize(vec3(0, 1, 1));
  vec3 light_vector = normalize(sp_light_pos - fs_position);
  vec3 rvec = normalize(vec3(0, 1, 1) + sp_light_pos);

  float diffuse = max(dot(fs_normal, light_vector), 0.01);
  float specular = max(dot(fs_normal, rvec), 0.01);
  specular = pow(specular, f_specular_intensity) * f_specular_alpha;
  vec4 ambient = vec4(0.01, 0.01, 0.01, 1.0);
  vec3 gamma = vec3(1.0/2.2, 1.0/2.2, 1.0/2.2);
  vec4 linear_color =  vec4(v_color, 1.0) * diffuse + vec4(1.0, 1.0, 1.0, 1.0) * specular;

  color = vec4(pow(linear_color.r, gamma.r),
               pow(linear_color.g, gamma.g),
               pow(linear_color.b, gamma.b), 1.0);
}
)"
