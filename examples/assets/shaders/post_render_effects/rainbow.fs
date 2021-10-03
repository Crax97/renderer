#version 460 core

in vec2 fs_texcoord;

uniform sampler2D diffuse;
uniform float time;

out vec4 color;

float zosin(float x) { return (sin(x) + 1.0) * 0.5; }

void main() {
  vec4 tex_color = texture(diffuse, fs_texcoord);
  float t = time / 3.0;
  tex_color.r = zosin(tex_color.r + t);
  tex_color.r = zosin(tex_color.g + t);
  tex_color.r = zosin(tex_color.b + t);
  color = tex_color;
}