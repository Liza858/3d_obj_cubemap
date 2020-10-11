#version 330 core


in vec3 out_normal;
in vec3 out_position;
in vec2 out_tex_coords;

uniform sampler2D obj_texture;
uniform samplerCube cubemap_texture;

uniform float n_env;
uniform float n_obj;
uniform float color_intensivity;
uniform vec3 camera_position;


void main() {

    vec3 dir = normalize(out_position - camera_position);
    vec3 normal = normalize(out_normal);

    float cos_refl = dot(-dir, normal);
    float sin2 = n_env / n_obj * n_env / n_obj * (1 - pow(cos_refl, 2));
    float cos_refr = pow(1 - sin2, 0.5);

    vec3 refl_angle = reflect(dir, normalize(normal));
    vec3 refr_angle = refract(dir, normalize(normal), n_env / n_obj);

    vec4 reflect_part = vec4(texture(cubemap_texture, refl_angle).rgb, 1.0);
    vec4 refract_part = vec4(texture(cubemap_texture, refr_angle).rgb, 1.0);
    vec4 texture_color =  vec4(texture(obj_texture, out_tex_coords).rgb, 1.0);


    float r1 = pow((n_env *cos_refr - n_obj *cos_refl) / (n_env * cos_refr + n_obj * cos_refl), 2);
    float r2 =  pow((n_obj * cos_refr - n_env * cos_refl) / (n_obj * cos_refr + n_env * cos_refl), 2);
 
    float r = (r1 + r2) / 2;
    if (sin2 > 1) {
        r = 1;
    }
    float t = 1 - r;
    vec4 rt = r * reflect_part + t * refract_part;

    gl_FragColor = mix(rt, texture_color, color_intensivity);
}