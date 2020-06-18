#include <GLFW/glfw3.h>

class ControlSystem {
 public:
  void OnMouseMove(double xpos, double ypos);
  void OnMouseClick(int button, int action, int mods);
  void OnKey(int key, int scancode, int action, int mods);

 private:
  bool first_mouse_ = true;
  double mouse_last_x_ = 0;
  double mouse_last_y_ = 0;
  bool enable_mouse_move_ = false;
};

void ControlSystem::OnMouseMove(double xpos, double ypos) {
  if (!enable_mouse_move_) return;

  if (first_mouse_) {
    first_mouse_ = false;
    mouse_last_x_ = xpos;
    mouse_last_y_ = ypos;
    return;
  }

  double delta_x = xpos - mouse_last_x_;
  double delta_y = ypos - mouse_last_y_;

  mouse_last_x_ = xpos;
  mouse_last_y_ = ypos;

  world->GetCamera()->ProcessMouse(delta_x, delta_y);
}

void ControlSystem::OnMouseClick(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    enable_mouse_move_ = true;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    enable_mouse_move_ = false;
    first_mouse_ = true;
  }
}

void ControlSystem::OnKey(int key, int scancode, int action, int mods) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  double delta_time = current_frame_time - last_frame_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyForward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyBackward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyLeft, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyRight, delta_time);
  }
}
