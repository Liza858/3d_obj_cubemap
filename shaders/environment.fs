#version 330 core
out vec4 o_frag_color;

in vec3 tex_coords;
uniform samplerCube environment;

void main()
{    
    o_frag_color = texture(environment, tex_coords);
}
