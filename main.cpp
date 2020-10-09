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
#include "environment.h"
#include "textures.h"
#include "object_loader.h"

float mouse_offset_x = 0.0;
float mouse_offset_y = 0.0;

float angle_x = 0.0;
float angle_y = 0.0;

int zoom_sensitivity = 10;
float zoom = 0.1;

double mouse_prev_x = 0.0;
double mouse_prev_y = 0.0;

bool button_is_pressed = false;

static void glfw_error_callback(int error, const char *description)
{
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
   zoom = zoom + yoffset * zoom_sensitivity / 10000.0;
   if (zoom < 0) {
      zoom = 0;
   }
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
            mouse_prev_x = x;
            mouse_prev_y = y;
            angle_x -= mouse_offset_x * 0.25;
            if (angle_y - mouse_offset_y * 0.25 > 89.9) {
               angle_y = 89.9;
            } else if (angle_y - mouse_offset_y * 0.25 < -89.9) {
               angle_y = -89.9;
            } else {
               angle_y -= mouse_offset_y * 0.25;
            }
         } else {
            button_is_pressed = true;
            mouse_prev_x = x - mouse_offset_x;
            mouse_prev_y = y - mouse_offset_y;
         }
      } else if (action == GLFW_RELEASE) {
          button_is_pressed = false;
      }
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

   glfwSetScrollCallback(window, scroll_callback);

   shader_t env_shader("environment.vs", "environment.fs");
   shader_t obj_shader("obj.vs", "obj.fs");

   std::array<std::string, 6> textures_src = {
      "../environment/right.jpg",
      "../environment/left.jpg",
      "../environment/top.jpg",
      "../environment/bottom.jpg",
      "../environment/front.jpg",
      "../environment/back.jpg"
   };

   GLuint cubemap_texture = CubemapTextureLoader::load(textures_src);
   GLuint obj_textute = TextureLoader::load("../objects/fish.jpg");

   Environment env;
   Object obj = ObjLoader::load("../objects/", "../objects/fish.obj");


   // Setup GUI context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);
   ImGui::StyleColorsDark();

   static float reflection = 0.15;
   static float refraction = 0.5;
   static float refraction_coeff = 1.00 / 1.52;

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

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
      ImGui::SliderInt("zoom sensitivity, %", &zoom_sensitivity, 0, 100);
      ImGui::SliderFloat("refraction_coeff", &refraction_coeff, 0, 1);
      ImGui::SliderFloat("reflection_intensivity", &reflection, 0, 1);
      ImGui::SliderFloat("refraction_intensivity", &refraction, 0, 1);
      ImGui::End();

        
      // Mouse button press action
      if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive())) {
          auto mouse_action = glfwGetMouseButton(window, 0);
          mouse_button_callback(window, mouse_action);
      }

      glm::vec4 camera(0, 0, 1, 1);
      auto x_rot = glm::rotate(glm::mat4(1), glm::radians(angle_x), glm::vec3(0, 1, 0));
      camera = x_rot * camera;
      camera = glm::rotate(glm::mat4(1), glm::radians(angle_y), glm::vec3(x_rot * glm::vec4(1, 0, 0, 1))) * camera;
      auto model = glm::scale(glm::vec3(zoom, zoom, zoom));
      model = glm::rotate(model, 3.14f / 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
      auto view = glm::lookAt<float>(glm::vec3(camera.x, camera.y, camera.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
      auto projection = glm::perspective<float>(90, float(display_w) / display_h, 0.1, 100);


      glClear(unsigned(GL_COLOR_BUFFER_BIT) | unsigned(GL_DEPTH_BUFFER_BIT));


      // отключаем тест глубины, чтобы все рисовалось поверх environment
      glDepthMask(GL_FALSE);
      env_shader.use();
      env_shader.set_uniform("projection", glm::value_ptr(projection));
      env_shader.set_uniform("view", glm::value_ptr(view));
      env_shader.set_uniform("environment", 1);
      env.render(env_shader, cubemap_texture);
      glDepthMask(GL_TRUE);


      obj_shader.use();
      obj_shader.set_uniform("model", glm::value_ptr(model));
      obj_shader.set_uniform("view", glm::value_ptr(view));
      obj_shader.set_uniform("projection", glm::value_ptr(projection));
      obj_shader.set_uniform("camera_position", camera.x, camera.y, camera.z);
      obj_shader.set_uniform("refraction_coeff", refraction_coeff);
      obj_shader.set_uniform("reflection_intensivity", reflection);
      obj_shader.set_uniform("refraction_intensivity", refraction);
      obj_shader.set_uniform("obj_texture", 0);
      obj_shader.set_uniform("cubemap_texture", 1);
      obj.render(obj_shader, obj_textute, cubemap_texture);


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
