#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"

#include "camera.h"
#include "cloth.h"
#include "hand_mesh.h"
#include "light.h"
#include "mesh.h"
#include "misc.h"
#include "model.h"
#include "object.h"
#include "shader.h"
#include "stb_image.h"
#include "time.h"

using namespace std;

// --------------------
// Classes
// --------------------
class MainContext {
public:
  static const int HEIGHT = 600;
  static const int WIDTH = 800;

  GLFWwindow *window;

  // Frame count and time information
  int num_frames = 0;
  double realtime_ms = 0.0f;
  double last_time_ms = 0.0f;
  double delta_ms = 0.0f;

  bool debug = false;
  bool move_light = false;
  bool light_directional = false;
  bool light_spotlight = false;
  bool pendulumSpotLights = false;
  bool drawBorder = false;

  Timer graphicsTimer;
  Timer physicsTimer;
  Timer inputTimer;
  VirtualClock graphicsClock;
  VirtualClock physicsClock;
  GlfwClock realtimeClock;
  FrameCounter physicsFrameCounter;
  FrameCounter graphicsFrameCounter;
  FrameCounter realtimeFrameCounter;
  FrameCounter inputFrameCounter;

  int _do_input;
  int _do_physics;
  int _do_render;

  MainContext() {
    graphicsTimer.setIntervalMillis(1000 / 30.0);
    physicsTimer.setIntervalMillis(1000 / 30.0);
    inputTimer.setIntervalMillis(1000 / 30.0);
  }

  void updateTime() {
    realtime_ms = realtimeClock.getTimeMillis();
    delta_ms = realtime_ms - last_time_ms;
    last_time_ms = realtime_ms;

    graphicsTimer.tickWithClock(graphicsClock);
    physicsTimer.tickWithClock(physicsClock);
    inputTimer.tickWithClock(realtimeClock);
    realtimeFrameCounter.tick(realtime_ms);
    _do_input = inputTimer.passed();
    _do_render = graphicsTimer.passed();
    _do_physics = physicsTimer.passed();

    // if (realtimeFrameCounter.tick(realtime_ms)) {
    //  printf("%10.2fs l:%5.0f fps, i:%3.0f fps, r:%3.0f fps, p:%3.0f fps, "
    //         "delta:%8fs\n",
    //         realtime_ms / 1000, realtimeFrameCounter.fps(),
    //         inputFrameCounter.fps(), graphicsFrameCounter.fps(),
    //         physicsFrameCounter.fps(), delta_ms / 1000);
    //}
  }

  int do_input() { return _do_input; }
  int do_physics() { return _do_physics; }
  int do_render() { return _do_render; }
  float delta_time() {
    // return delta_ms;
    // printf("%f\n", inputTimer.getIntervalMillis());
    return (inputTimer.getIntervalMillis() / 1000);
  };

  float aspect_ratio() const {
    return float(MainContext::WIDTH) / float(MainContext::HEIGHT);
  }
};

class Input {
public:
  bool keys[1024];
  bool prevKeys[1024];
  bool shift = false;
  bool ctrl = false;

  double mouse_x;
  double mouse_y;
  double prev_mouse_x;
  double prev_mouse_y;
  bool mouse_left;
  bool mouse_right;

  static Input *get() {
    static Input input;
    return &input;
  }

private:
  Input() {
    for (int i = 0; i < 1024; i++) {
      keys[i] = false;
      prevKeys[i] = false;
    }
  }
  ~Input() {}
};

std::ostream &operator<<(std::ostream &os, const glm::vec3 &v) {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

// Function Defs
// ----------------------------------
// void process_input(const MainContext& context, const Input& input, Camera*
// cam);
void process_input(MainContext &context, const Input &input, Camera *cam);

// Input callbacks
// ------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow *window, double x, double y);
void error_callback(int error, const char *description);

// --------------------------
// CALLBACKS
// --------------------------
void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  Input *input = Input::get();

  input->shift = mods & GLFW_MOD_SHIFT;
  input->ctrl = mods & GLFW_MOD_CONTROL;

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    // input->prevKeys[key] = input->keys[key];
    input->keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    // input->prevKeys[key] = input->keys[key];
    input->keys[key] = false;
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  return;
  Input *input = Input::get();

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

  // printf("Mouse: %.2f,%.2f\n", xpos, ypos);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  return;
  Input *input = Input::get();
  if (action == GLFW_PRESS) {
    input->mouse_left = button == GLFW_MOUSE_BUTTON_LEFT;
    input->mouse_right = button == GLFW_MOUSE_BUTTON_RIGHT;
  } else {
    input->mouse_left = false;
    input->mouse_right = false;
  }
}

void process_input(MainContext &context, const Input &input, Camera *cam) {
  if (input.keys[GLFW_KEY_W]) {
    cam->process_input(Camera::FORWARD, context.delta_time());
  } else if (input.keys[GLFW_KEY_S]) {
    cam->process_input(Camera::BACKWARD, context.delta_time());
  }

  if (input.keys[GLFW_KEY_D]) {
    cam->process_input(Camera::RIGHT, context.delta_time());
  } else if (input.keys[GLFW_KEY_A]) {
    cam->process_input(Camera::LEFT, context.delta_time());
  }

  if (input.keys[GLFW_KEY_SPACE]) {
    if (input.shift) {
      cam->process_input(Camera::DOWN, context.delta_time());
    } else {
      cam->process_input(Camera::UP, context.delta_time());
    }
  }

  if (input.keys[GLFW_KEY_LEFT]) {
    cam->process_rotate(Camera::YAW_LEFT, context.delta_time());
  } else if (input.keys[GLFW_KEY_RIGHT]) {
    cam->process_rotate(Camera::YAW_RIGHT, context.delta_time());
  } else if (input.keys[GLFW_KEY_UP]) {
    cam->process_rotate(Camera::PITCH_UP, context.delta_time());
  } else if (input.keys[GLFW_KEY_DOWN]) {
    cam->process_rotate(Camera::PITCH_DOWN, context.delta_time());
  } else if (input.keys[GLFW_KEY_PERIOD]) {
    cam->process_rotate(Camera::ROLL_CLOCKWISE, context.delta_time());
  } else if (input.keys[GLFW_KEY_COMMA]) {
    cam->process_rotate(Camera::ROLL_COUNTER_CLOCKWISE, context.delta_time());
  }

  if (input.keys[GLFW_KEY_T]) {
    context.debug = !context.debug;
  }
  if (input.keys[GLFW_KEY_L]) {
    context.move_light = !context.move_light;
  }
  if (input.keys[GLFW_KEY_B]) {
    context.light_directional = true;
  }
  if (input.keys[GLFW_KEY_N]) {
    context.light_directional = false;
  }
  if (input.keys[GLFW_KEY_G]) {
    context.light_spotlight = true;
  }
  if (input.keys[GLFW_KEY_H]) {
    context.light_spotlight = false;
  }

  if (input.keys[GLFW_KEY_1]) {
    cam->reset();
  }
  if (input.keys[GLFW_KEY_2]) {
    cam->reset();
    cam->translator.move_to(glm::vec3(0, 0, 3.f));
    cam->rotator.rotateY(0);
  }
  if (input.keys[GLFW_KEY_3]) {
    cam->reset();
    cam->translator.move_to(glm::vec3(3, 3, 3));
    // cam->rotator.reset();
    cam->rotator.rotateY(-45);
    cam->rotator.rotateX(-45);
  }
  if (input.keys[GLFW_KEY_4]) {
    cam->reset();
    cam->translator.move_to(glm::vec3(-3, 3, -3));
    // cam->rotator.reset();
    cam->rotator.rotateY(180);
    cam->rotator.rotateY(-45);
    cam->rotator.rotateX(-45);
  }

  // double x_offset = input.mouse_x - input.prev_mouse_x;
  // double y_offset = input.prev_mouse_y - input.mouse_y;
  // cam->process_mouse(x_offset, y_offset);
}

void updatePendulumSpotLights(MainContext &ctx, vector<Light> &spotLights) {
  glm::vec3 attachPoint = glm::vec3(0, 5, 0);
  float theta_0s[] = {25, -25, 45, 65};
  float Ls[] = {3, 2, 0.5, 1};
  for (int i = 0; i < spotLights.size(); i++) {
    Light &sl = spotLights[i];
    float t = ctx.physicsClock.getTimeSecs() * 1.5;
    float theta_0 = glm::radians(theta_0s[i]);
    float g = 9.81;
    float L = Ls[i];
    float w = glm::sqrt(g / L);
    float theta_t = theta_0 * cos(w * t);

    sl.rotation.reset();
    sl.rotation.rotateX(90.0f);
    sl.rotation.rotateY(glm::degrees(theta_t));
    sl.position.move_to(attachPoint + sl.rotation.front() * L);
  }
}

void drawImGui(MainContext &ctx, Camera &cam) {
  // printf("%10.2fs l:%5.0f fps, i:%3.0f fps, r:%3.0f fps, p:%3.0f fps, "
  //     "delta:%8fs\n",
  //     realtime_ms / 1000, realtimeFrameCounter.fps(),
  //     inputFrameCounter.fps(), graphicsFrameCounter.fps(),
  // physicsFrameCounter.fps(), delta_ms / 1000);
  // absl::StrFormat("%10.2fs delta:%8fs", ctx.realtime_ms / 1000,
  // ctx.delta_ms); absl::StrFormat("%10.2fs l:%5.0f fps, i:%3.0f fps, r:%3.0f
  // fps, p:%3.0f fps, "
  ImGui::Begin("Information");
  ImGui::Text("Time: %5.2fs Delta: %8fs", ctx.realtime_ms / 1000, ctx.delta_ms);
  ImGui::Text("l:%5.0f fps, i:%3.0f fps, r:%3.0f fps, p:%3.0f fps",
              ctx.realtimeFrameCounter.fps(), ctx.inputFrameCounter.fps(),
              ctx.graphicsFrameCounter.fps(), ctx.physicsFrameCounter.fps());
  ImGui::BeginChild("Camera", ImVec2(250, 100), true);
  ImGui::Text("Camera");
  ImGui::Text(" pos  (%5.2f,%5.2f,%5.2f)", cam.translator.pos.x,
              cam.translator.pos.y, cam.translator.pos.z);
  ImGui::Text(" front(%5.2f,%5.2f,%5.2f)", cam.rotator.front().x,
              cam.rotator.front().y, cam.rotator.front().z);
  ImGui::Text(" up   (%5.2f,%5.2f,%5.2f)", cam.rotator.up().x,
              cam.rotator.up().y, cam.rotator.up().z);
  ImGui::Text(" right(%5.2f,%5.2f,%5.2f)", cam.rotator.right().x,
              cam.rotator.right().y, cam.rotator.right().z);
  ImGui::EndChild();
  if (ImGui::Button("F1")) {
    cam.reset();
  }
  ImGui::SameLine();
  ImGui::Checkbox("Debug", &ctx.debug);
  ImGui::End();
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

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Important in Mac
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  MainContext ctx;
  Camera cam;
  cam.reset();

  ctx.window = glfwCreateWindow(MainContext::WIDTH, MainContext::HEIGHT,
                                "Cloth", NULL, NULL);
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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(ctx.window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Viewport
  glViewport(0, 0, MainContext::WIDTH, MainContext::HEIGHT);
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_STENCIL_TEST);
  glPointSize(5);
  glLineWidth(5);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glPolygonMode(GL_FRONT, GL_FILL);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Shader reading
  Shader shader("shaders/vs.glsl", "shaders/fs.glsl");
  Shader light_shader("shaders/vs.glsl", "shaders/light_fs.glsl");
  Shader debug_shader("shaders/debug_vs.glsl", "shaders/debug_fs.glsl",
                      "shaders/debug_gs.glsl");
  Shader shaderSingleColor("shaders/vs.glsl",
                           "shaders/shader_single_color_fs.glsl");
  printf("Program start.\n");

  // Load a texture
  // unsigned int texture1 = load_texture("res/container.jpg");
  // unsigned int texture2 = load_texture("res/awesomeface.png");
  unsigned int texture3 = load_texture("res/container2.png");
  unsigned int texture4 = load_texture("res/container2_specular.png");
  unsigned int texture5 = load_texture("res/matrix.jpg");

  // Objects and meshes
  Mesh cubeMesh = create_cube_mesh2({
      {"texture_diffuse", texture3}, {"texture_specular", texture4},
      //{"texture_emission", texture5},
  });
  Model cube = Model(cubeMesh);
  // Model backpack = Model("res/backpack/backpack.obj");

  vector<Object> objects;
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
  for (int i = 0; i < 10; i++) {
    // Object o = Object(cube);
    Object o = Object(cube);
    o.position.move_to(cubePositions[i]);
    float angle = 20.0f * i;
    o.rotation.rotate(glm::vec3(1.0f, 0.3f, 0.5f), angle);
    objects.push_back(o);
  }
  objects.clear();
  // objects.push_back(Object(backpack));
  objects.push_back(Object(cube));
  objects.push_back(Object(cube));
  // objects.push_back(Object(cube));
  objects[0].position.move_to(glm::vec3(1, 0, 0));
  objects[1].position.move_to(glm::vec3(-1, 0, 0));
  // objects[2].position.move_to(glm::vec3(0, 0, 3));
  // objects[3].position.move_to(glm::vec3(0, 0, -3));
  // objects.clear();
  // objects.push_back(Object(cube));
  // objects.push_back(Object(cube));
  // objects.push_back(Object(cube));
  // objects.push_back(Object(cube));
  // objects[1].position.move_to(glm::vec3(1,0,0));
  // objects[1].rotation.rotateX(20);
  // objects[2].position.move_to(glm::vec3(2,0,0));
  // objects[2].rotation.rotateY(20);
  // objects[3].position.move_to(glm::vec3(3,0,0));
  // objects[3].rotation.rotateZ(20);

  vector<Light> dirLights;
  vector<Light> spotLights;
  vector<Light> pointLights;

  dirLights.push_back(Light(cube, Light::DIRECTIONAL));
  dirLights[0].rotation.rotateY(90.0f);
  dirLights[0].position.move_to(-dirLights[0].rotation.front());
  dirLights[0].scale.scale(0.1);

  spotLights.push_back(Light(cube, Light::SPOT));
  spotLights.push_back(Light(cube, Light::SPOT));
  spotLights.push_back(Light(cube, Light::SPOT));
  spotLights.push_back(Light(cube, Light::SPOT));
  spotLights[0].scale.scale(0.1);
  spotLights[1].scale.scale(0.1);
  spotLights[2].scale.scale(0.1);
  spotLights[3].scale.scale(0.1);
  // spotLights[0].position.move_to(glm::vec3(0, 1.5, 0));
  // spotLights[0].rotation.rotateX(90.0f);

  glm::vec3 pointLightPositions[] = {
      glm::vec3(1, 1, 1), glm::vec3(0.7f, 0.2f, 2.0f),
      glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(-4.0f, 2.0f, -12.0f),
      glm::vec3(0.0f, 0.0f, -3.0f)};
  for (int i = 0; i < 4; i++) {
    Light l = Light(cube);
    l.position.move_to(pointLightPositions[i]);
    l.scale.scale(0.1);
    pointLights.push_back(l);
  }

  dirLights.clear();
  spotLights.clear();
  //pointLights.clear();

  // Setup shaders
  shader.use();
  shader.setInt("numDirLights", dirLights.size());
  shader.setInt("numSpotLights", spotLights.size());
  shader.setInt("numPointLights", pointLights.size());

  // Main loop
  while (!glfwWindowShouldClose(ctx.window)) {
    ctx.updateTime();
    // cout << cam.rotator << endl;

    // Input
    if (ctx.do_input()) {
      ctx.inputFrameCounter.tick(ctx.realtime_ms);
      process_input(ctx, *Input::get(), &cam);
    }

    // Physics
    if (ctx.do_physics()) {
      if (ctx.physicsFrameCounter.tick(ctx.realtime_ms) && false) {
        printf("Do physics: fps: %f, time: %f, rate = %f \n",
               ctx.physicsFrameCounter.fps(),
               ctx.physicsTimer.getCurrentMillis(), ctx.physicsClock.getRate());
      }
      if (ctx.move_light && pointLights.size() > 0) {
        float time_secs = ctx.physicsClock.getTimeSecs();
        // float time_secs = current_time_ms / 1000;
        pointLights[0].position.move_to(glm::vec3(
            cos(time_secs * 1.5) * 2.5, 1.25f, sin(time_secs * 1.5) * 2.5));
      }
      if (ctx.light_spotlight && spotLights.size() > 0) {
        spotLights[0].position.move_to(
            (cam.translator.pos - 2.0f * cam.rotator.front()));
        spotLights[0].rotation = cam.rotator;
      }

      if (ctx.pendulumSpotLights) {
        updatePendulumSpotLights(ctx, spotLights);
      }

      // Update all the lights
      for (int i = 0; i < dirLights.size(); i++) {
        dirLights[i].use(&shader, i);
      }
      for (int i = 0; i < spotLights.size(); i++) {
        spotLights[i].use(&shader, i);
      }
      for (int i = 0; i < pointLights.size(); i++) {
        pointLights[i].use(&shader, i);
      }
    }

    // Render
    if (ctx.do_render()) {
      if (ctx.graphicsFrameCounter.tick(ctx.realtime_ms) && false) {
        printf("Do graphics: fps: %f, time: %f, rate = %f \n",
               ctx.graphicsFrameCounter.fps(),
               ctx.graphicsTimer.getCurrentMillis(),
               ctx.graphicsClock.getRate());
      }

      glClearColor(0, 0, 0, 0);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // Draw
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      drawImGui(ctx, cam);
      ImGui::Render();

      glEnable(GL_STENCIL_TEST);
      glStencilMask(0xFF); // enable writing to the stencil buffer

      shader.use();
      cam.use(ctx.aspect_ratio(), &shader);
      glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
      glStencilOpSeparate(GL_FRONT, GL_REPLACE, GL_REPLACE, GL_REPLACE);
      glStencilFuncSeparate(GL_BACK, GL_NEVER, 1, 0xFF); // all fragments should pass the stencil test
      glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
      for (Object &obj : objects) {
        obj.draw(shader);
      }

      if (ctx.drawBorder) {
        shaderSingleColor.use();
        shaderSingleColor.set4Float("customColor", 0.3, 0,0.5, 1.0);
        cam.use(ctx.aspect_ratio(), &shaderSingleColor);
        //glStencilMask(0x00);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFuncSeparate(GL_BACK, GL_NEVER, 1, 0xFF); // all fragments should pass the stencil test
        glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, 1, 0xFF); // all fragments should pass the stencil test
        for (Object &obj : objects) {
          obj.scale.scale(1.05f);
          obj.draw(shaderSingleColor);
          obj.scale.scale(1.0f);
        }
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
      }

      // Draw lights
      light_shader.use();
      cam.use(ctx.aspect_ratio(), &light_shader);
      for (Light &obj : pointLights) {
        obj.draw(light_shader);
      }
      for (Light &obj : spotLights) {
        obj.draw(light_shader);
      }
      for (Light &obj : dirLights) {
        obj.draw(light_shader);
      }

      // Draw debug if requested
      if (ctx.debug) {
        debug_shader.use();
        cam.use(ctx.aspect_ratio(), &debug_shader);
        for (Object &obj : objects) {
          obj.draw(debug_shader);
        }
      }

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      glfwSwapBuffers(ctx.window);
    }

    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(ctx.window);
  glfwTerminate();
  return 0;
}
