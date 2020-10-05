#version 330 core

out vec4 o_frag_color;



struct vx_output_t
{
    vec3 color;
    vec4 pos;
};

in vx_output_t v_out;


uniform vec2 center_position;
uniform float half_width;
uniform int max_iter; 
uniform int screen_w;
uniform int screen_h;
uniform sampler1D u_tex;


void main()
{
    vec2 fractal_center = vec2(center_position.x, center_position.y);
    float fractal_half_width = half_width;
    vec2 area_x = vec2(-2.0, 1.0);
    vec2 area_y = vec2(-1.0, 1.0);
    float area_w = area_x.y - area_x.x;
    float area_h = area_y.y - area_y.x;

    float fractal_half_height = fractal_half_width / area_w * area_h;

    fractal_half_height = fractal_half_height * screen_w / screen_h;

    float w_scale = area_w / (area_h * fractal_half_width);
    float h_scale = area_h / (area_h * fractal_half_height);

    vec2 bottom_left = vec2(fractal_center.x - fractal_half_width, fractal_center.y - fractal_half_height);

   
    float x = v_out.pos.x - bottom_left.x + area_x.x;
    float y = v_out.pos.y - bottom_left.y + area_y.x;

    vec2 c = vec2(area_x.x + (x - area_x.x) * w_scale, area_y + (y - area_y.x) * h_scale);
    vec2 z = vec2(0.0, 0.0);

    int iter = 0;

    while (iter < max_iter)
    {
        float x = z.x * z.x - z.y * z.y + c.x;
        float y = 2.0 * z.x * z.y + c.y;

        if (x * x + y * y > 4.0) {
            break;
        }

        z = vec2(x, y);

        iter++;
    }

    o_frag_color = o_frag_color = vec4((iter == max_iter? vec3(0.0, 0.0, 0.0) : texture(u_tex, iter * 1.0 / max_iter).rgb), 1.0);;
  
}
