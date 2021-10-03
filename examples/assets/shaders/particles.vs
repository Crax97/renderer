#version 460 core

in vec3 vertex;
in vec2 texcoord;
in vec3 normal;

layout(location = 10) in vec3 initial_velocity;
layout(location = 11) in mat3 rotation_mat;
layout(location = 14) in vec3 color;
layout(location = 15) in float opacity;
in float lifetime;
layout(location = 17) in float rotation;

out vec2 fs_texcoord;
uniform mat4 mvp;
uniform float time;
uniform vec3 gravity;

void main() {
    float t = lifetime;
    vec3 position = vertex + initial_velocity * t + 0.5 * gravity * t * t;
    gl_Position = mvp * vec4(position, 1.0);
    fs_texcoord = texcoord;
    fs_texcoord.y = 1.0f - fs_texcoord.y;
}