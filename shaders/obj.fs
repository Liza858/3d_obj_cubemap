#version 330 core


in vec3 out_normal;
in vec3 out_position;
in vec2 out_tex_coords;

uniform sampler2D obj_texture;
uniform samplerCube cubemap_texture;

uniform float reflection;
uniform float refraction;
uniform vec3 camera_position;


void main() {

    vec3 dir = normalize(out_position - camera_position);
    vec3 normal = normalize(out_normal);
    vec3 refl_angle = reflect(dir, normalize(normal));
    vec3 refr_angle = refract(dir, normalize(normal), refraction);
    vec4 reflect_part = vec4(texture(cubemap_texture, refl_angle).rgb, 1.0);
    vec4 refract_part = vec4(texture(cubemap_texture, refr_angle).rgb, 1.0);
    vec4 texture_color =  vec4(texture(obj_texture, out_tex_coords).rgb, 1.0);
    vec4 v =  mix(reflect_part, refract_part, refraction);
    gl_FragColor = texture_color;
    
}