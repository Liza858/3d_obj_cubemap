#include <iostream>
#include <vector>
#include <chrono>
#include <utility>
	

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_shader.h"

float mouse_offset_x = 0.0;
float mouse_offset_y = 0.0;

float center_position_x = 0.0;
float center_position_y = 0.0;
float half_width = 0.5;
int zoom_sensitivity = 10;

double mouse_prev_x = 0.0;
double mouse_prev_y = 0.0;

bool button_is_pressed = false;

static void glfw_error_callback(int error, const char *description)
{
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}


float texture_colors[] = {   
   1.0f,   0.0f,   0.0f,
   0.0f,   1.0f,   0.0f,
   0.0f,   0.0f,   1.0f,
};

const int texture_colors_count = 3;

void create_triangles(GLuint &vbo, GLuint &vao, GLuint &ebo)
{
   // create the triangle
   float triangle_vertices[] = {
       -1.0f, -1.0f, 0.0f,	
       1.0f, 0.0f, 0.0f,	 

       -1.0f, 1.0f, 0.0f,  
       0.0f, 1.0f, 0.0f,	 

       1.0f, 1.0f, 0.0f, 
       0.0f, 0.0f, 1.0f,	 

       1.0f, -1.0f, 0.0f, 
       0.0f, 0.0f, 1.0f,	
   };
   unsigned int triangle_indices[] = { 0, 1, 2, 2, 3, 0 };
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

std::pair<float, float> get_mouse_local_coords(int x, int y, int display_w, int display_h) {
    float x_local = -1 * (1.0 * (display_w - x) / display_w * 2 - 1);
    float y_local = 1.0 * (display_h - y) / display_h * 2 - 1;
    return std::pair<float, float>(x_local, y_local);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
   half_width = half_width + half_width * yoffset * zoom_sensitivity / 100.0;
   double x, y;
   glfwGetCursorPos(window, &x, &y);
   int width, height;
   glfwGetWindowSize(window, &width, &height);
   auto local_coords = get_mouse_local_coords(x, y, width, height);
   center_position_x = center_position_x - (local_coords.first -  center_position_x) * yoffset * zoom_sensitivity / 100.0;;
   center_position_y = center_position_y - (local_coords.second -  center_position_y) * yoffset * zoom_sensitivity / 100.0;;
}


void mouse_button_callback(GLFWwindow* window, int action)
{
   int display_w, display_h;
   glfwGetWindowSize(window, &display_w, &display_h);
    if (action == GLFW_PRESS) {
         double x, y;
         glfwGetCursorPos(window, &x, &y);
         if (button_is_pressed) {
            mouse_offset_x = x - mouse_prev_x;
            mouse_offset_y = y - mouse_prev_y;  
            center_position_x = center_position_x + 2.0 * mouse_offset_x / display_w;
            center_position_y = center_position_y - 2.0 * mouse_offset_y / display_h;
            mouse_prev_x = x;
            mouse_prev_y = y;
         } else {
            button_is_pressed = true;
            mouse_prev_x = x - mouse_offset_x;
            mouse_prev_y = y - mouse_offset_y;
         }
      } else if (action == GLFW_RELEASE) {
          button_is_pressed = false;
      }
}

void create_texture(GLuint & texture)
{
   glGenTextures(1, &texture);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_1D, texture);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, texture_colors_count, 0, GL_RGB, GL_FLOAT, texture_colors);
   glBindTexture(GL_TEXTURE_1D, 0);
}


int main(int, char **)
{
   // Use GLFW to create a simple window
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit())
      return 1;


   // GL 3.3 + GLSL 330
   const char *glsl_version = "#version 330";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

   // Create window with graphics context
   GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
   if (window == NULL)
      return 1;
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
   if (glewInit() != GLEW_OK)
   {
      std::cerr << "Failed to initialize OpenGL loader!\n";
      return 1;
   }

   // texture 
   GLuint texture;
   create_texture(texture);

   // events
   glfwSetScrollCallback(window, scroll_callback);

   // create our geometries
   GLuint vbo, vao, ebo;
   create_triangles(vbo, vao, ebo);

   // init shader
   shader_t triangles_shader("simple-shader.vs", "simple-shader.fs");

   // Setup GUI context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);
   ImGui::StyleColorsDark();


   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Get windows size
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);

      // Set viewport to fill the whole window area
      glViewport(0, 0, display_w, display_h);

      // Fill background with solid color
      glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT);

      // Gui start new frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // GUI
      ImGui::Begin("Settings");
      static int max_iterations = 30;
      ImGui::SliderInt("max iterations", &max_iterations, 0, 1000);
      ImGui::InputFloat("center x", &center_position_x);
      ImGui::InputFloat("center y", &center_position_y);
      ImGui::SliderInt("zoom sensitivity, %", &zoom_sensitivity, 0, 100);
      ImGui::ColorEdit3("first", texture_colors);
      ImGui::ColorEdit3("second", texture_colors + 3);
      ImGui::ColorEdit3("third", texture_colors + 6);
      ImGui::End();

      // Mouse button press action
      if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive())) {
          auto mouse_action = glfwGetMouseButton(window, 0);
          mouse_button_callback(window, mouse_action);
      }

      // Pass the parameters to the shader as uniforms
      triangles_shader.set_uniform("center_position", center_position_x, center_position_y);
      triangles_shader.set_uniform("half_width", half_width);
      triangles_shader.set_uniform("max_iter", max_iterations);
      triangles_shader.set_uniform("screen_w", display_w);
      triangles_shader.set_uniform("screen_h", display_h);


      triangles_shader.use();
      glActiveTexture(GL_TEXTURE0);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, texture_colors_count, 0, GL_RGB, GL_FLOAT, texture_colors);
      glBindTexture(GL_TEXTURE_1D, texture);

      // Bind vertex array = buffers + indices
      glBindVertexArray(vao);
      // Execute draw call
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);


      // Generate gui render commands
      ImGui::Render();

      // Execute gui render commands using OpenGL backend
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Swap the backbuffer with the frontbuffer that is used for screen display
      glfwSwapBuffers(window);
   }

   // Cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}
