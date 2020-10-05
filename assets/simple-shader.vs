#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

struct vx_output_t
{
    vec3 color;
    vec4 pos;
};
out vx_output_t v_out;

uniform float u_rotation;
uniform vec2 u_translation;
uniform mat4 u_mvp;

void main()
{
    vec2 rotated_pos;
    rotated_pos.x = u_translation.x + in_position.x * cos(u_rotation) - in_position.y * sin(u_rotation);
    rotated_pos.y = u_translation.y + in_position.x * sin(u_rotation) + in_position.y * cos(u_rotation);

    v_out.color = in_color;

    //gl_Position = u_mvp * vec4(rotated_pos.x, rotated_pos.y, in_position.z, 1.0);

    //gl_Position = u_mvp * vec4(3 * rotated_pos.x, 2 * rotated_pos.y, in_position.z, 1.0);
    gl_Position = vec4(in_position.x, in_position.y, in_position.z, 1.0);
    //gl_Position /= gl_Position.w;
    v_out.pos = gl_Position;
}
