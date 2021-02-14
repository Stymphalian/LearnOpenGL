#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <experimental/filesystem>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "shader.h"
#include "cloth.h"
#include "mesh.h"
#include "camera.h"
#include "object.h"
#include "hand_mesh.h"
#include "misc.h"

using namespace std;

// --------------------
// Classes
// --------------------
class MainContext {
public:
  static const int HEIGHT = 600;
  static const int WIDTH = 800;

  GLFWwindow* window;

  // Frame count and time information
  int num_frames = 0;
  double current_time_secs = 0.0f;
  double last_time_secs = 0.0f;
  double delta_time_secs = 0.0f;

  double delta_time = 0.0f;
  double last_frame = 0.0f;

  bool debug = false;

  MainContext() {}

  void update_frames() {
    num_frames += 1;
    current_time_secs = glfwGetTime();
    if (current_time_secs - last_time_secs >= 1.0f) {
      //printf("%10f %5d fps %10f\n", current_time_secs, num_frames, delta_time);
      num_frames = 0;
      delta_time_secs = current_time_secs - last_time_secs;
      last_time_secs += 1.0f;
    }

    delta_time = current_time_secs - last_frame;
    last_frame = current_time_secs;
  }

  float aspect_ratio() const {
    return float(MainContext::WIDTH) / float(MainContext::HEIGHT);
  }
};

class Input {
public:
  bool keys[1024];
  bool shift = false;
  bool ctrl = false;

  double mouse_x;
  double mouse_y;
  double prev_mouse_x;
  double prev_mouse_y;
  bool mouse_left;
  bool mouse_right;

  static Input* get() {
    static Input input;
    return &input;
  }
private:
  Input(){}
  ~Input(){}
};


// Function Defs
// ----------------------------------
//void process_input(const MainContext& context, const Input& input, Camera* cam);
void process_input(MainContext& context, const Input& input, Camera* cam);

// Input callbacks
// ------------------
void key_callback(GLFWwindow* window, int key, int scancode, 
                         int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);
void error_callback(int error, const char* description);

// --------------------------
// CALLBACKS
// --------------------------
void error_callback(int error, const char* description){
  fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, 
                         int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  Input* input = Input::get();

  input->shift = mods & GLFW_MOD_SHIFT;
  input->ctrl = mods & GLFW_MOD_CONTROL;
  if (action == GLFW_PRESS) {
    input->keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    input->keys[key] = false;
  }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  Input* input = Input::get();

  static bool first_mouse = true;
  if (first_mouse) {
    first_mouse = false;
    input->prev_mouse_x = xpos;
    input->prev_mouse_y = ypos;
  }
  input->prev_mouse_x = input->mouse_x;
  input->prev_mouse_y = input->mouse_y;
  input->mouse_x = xpos;
  input->mouse_y = ypos;

  //printf("Mouse: %.2f,%.2f\n", xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  Input* input = Input::get();
  if (action == GLFW_PRESS) {
    input->mouse_left = button == GLFW_MOUSE_BUTTON_LEFT;
    input->mouse_right = button == GLFW_MOUSE_BUTTON_RIGHT;
  } else {
    input->mouse_left = false;
    input->mouse_right = false;
  }
}

void process_input(
    MainContext& context, const Input& input,  Camera* cam) {
  if (input.keys[GLFW_KEY_W]) {
    cam->process_input(Camera::FORWARD, context.delta_time);
  } else if  (input.keys[GLFW_KEY_S]) {
    cam->process_input(Camera::BACKWARD, context.delta_time);
  }

  if (input.keys[GLFW_KEY_D]) {
    cam->process_input(Camera::RIGHT, context.delta_time);
  } else if  (input.keys[GLFW_KEY_A]) {
    cam->process_input(Camera::LEFT, context.delta_time);
  }
  
  if (input.keys[GLFW_KEY_SPACE]) {
    if (input.shift) {
      cam->process_input(Camera::DOWN, context.delta_time);
    } else {
      cam->process_input(Camera::UP, context.delta_time);
    }
  }

  if (input.keys[GLFW_KEY_LEFT]) {
    cam->rotate(Camera::YAW_LEFT, context.delta_time);
  } else if (input.keys[GLFW_KEY_RIGHT]) {
    cam->rotate(Camera::YAW_RIGHT, context.delta_time);
  } else if (input.keys[GLFW_KEY_UP]) {
    cam->rotate(Camera::PITCH_UP, context.delta_time);
  } else if (input.keys[GLFW_KEY_DOWN]) {
    cam->rotate(Camera::PITCH_DOWN, context.delta_time);
  }

  if (input.keys[GLFW_KEY_T]) {
    context.debug = !context.debug;
  }

  if (input.keys[GLFW_KEY_1]) {
    cam->pos = glm::vec3(0.0, 0.0, 3.0f);
    cam->front = glm::vec3(0.0, 0.0, -1.0f);
    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
  }
  if (input.keys[GLFW_KEY_2]) {
    cam->pos = glm::vec3(3.0f, 0.0, 0.0f);
    cam->front = glm::vec3(-1.0f, 0.0, 0.0);
    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
  }
  if (input.keys[GLFW_KEY_3]) {
    cam->pos = glm::vec3(0.0f, 3.0, 0.0f);
    cam->front = glm::vec3(0.0f, -1.0, 0.0);
    cam->yaw = -90.0f;
    cam->pitch = -89.0f;
  }
  if (input.keys[GLFW_KEY_4]) {
    cam->pos = glm::vec3(2.0f, 2.0f, 2.0f);
    cam->yaw = -135.0f;
    cam->pitch = -35;
    cam->update_camera_vectors();
  }

  //double x_offset = input.mouse_x - input.prev_mouse_x;
  //double y_offset = input.prev_mouse_y - input.mouse_y;
  //cam->process_mouse(x_offset, y_offset);
}

// --------------------------
// MAIN 
// --------------------------

int main(int argc, char **argv) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Important in Mac
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    MainContext ctx;
    Camera cam;

    ctx.window = glfwCreateWindow(MainContext::WIDTH, MainContext::HEIGHT, "Cloth", NULL, NULL);
    if (ctx.window == NULL) {
      fprintf(stderr, "Failed to create GLFW window\n");
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(ctx.window);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(ctx.window, key_callback);
    glfwSetCursorPosCallback(ctx.window, mouse_callback);
    glfwSetMouseButtonCallback(ctx.window, mouse_button_callback);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Viewport
    glViewport(0, 0, MainContext::WIDTH, MainContext::HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glPointSize(5);
    glLineWidth(5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Shader reading
    Shader shader("shaders/vs.glsl", "shaders/fs.glsl");
    Shader light_shader("shaders/vs.glsl", "shaders/light_fs.glsl");
    Shader debug_shader("shaders/debug_vs.glsl", 
                        "shaders/debug_fs.glsl", 
                        "shaders/debug_gs.glsl");
    printf("Program start.\n");

    // Load a texture
    unsigned int texture1 = load_texture("res/container.jpg");
    unsigned int texture2 = load_texture("res/awesomeface.png");

    // Objects and meshes
    Mesh cube = create_cube_mesh2({});

    vector<Object> objects;
    objects.push_back(Object(cube));
    objects.push_back(Object(cube));
    objects[0].position.move_to(glm::vec3(0.75, 0,0));
    //objects[0].rotation.rotateX(20);
    objects[1].position.move_to(glm::vec3(-0.75, 0,0));
    //objects[1].rotation.rotateY(45);
    //objects[1].scale.scale(1.0f, 0.7f, 0.25f);

    vector<Object> lights; 
    lights.push_back(Object(cube));
    lights[0].position.move_to(glm::vec3(1, 1,1));
    lights[0].scale.scale(0.2);

    // Setup shaders
    shader.use();
    shader.set3Float("objectColor", 1.0f, 0.5f, 0.31f);
    shader.set3Float("lightColor",  1.0f, 1.0f, 1.0f);
    shader.set3Float("lightPos", lights[0].position.pos);

    // Main loop
    while (!glfwWindowShouldClose(ctx.window)) {
      ctx.update_frames();
      process_input(ctx, *Input::get(), &cam);

      // Physics 
      //lights[0].position.move_to(
      //    glm::vec3(
      //      cos(ctx.current_time_secs*1.5)*2.5,
      //      0.0f, 
      //      sin(ctx.current_time_secs*1.5)*2.5));
      //shader.use();
      //shader.set3Float("lightPos", lights[0].position.pos);

      // Render
      glClearColor(0,0,0,0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      // Draw
      shader.use();
      cam.use(ctx.aspect_ratio(),  &shader);
      for (Object& obj: objects) {
        obj.draw(shader);
      }

      // Draw lights
      light_shader.use();
      cam.use(ctx.aspect_ratio(),  &light_shader);
      for (Object& obj: lights) {
        obj.draw(light_shader);
      }

      if (ctx.debug) {
        debug_shader.use();
        cam.use(ctx.aspect_ratio(), &debug_shader);
        for (Object& obj: objects) {
          obj.draw(debug_shader);
        }
      }

      glfwSwapBuffers(ctx.window);
      glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

