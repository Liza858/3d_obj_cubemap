#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out vec3 out_normal;
out vec3 out_position;
out vec2 out_tex_coords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

    out_normal = mat3(transpose(inverse(model))) * normal;
    out_position = vec3(model * vec4(position, 1.0));
    out_tex_coords = tex_coords;
    gl_Position =  projection * view * model * vec4(position, 1.0);
}