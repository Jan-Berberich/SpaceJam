#include "wizball.h"

void wbInputJoystickInit(WBJoystick* joystick, int joystick_id) {
    joystick->joystick_id = joystick_id;
    joystick->axes        = NULL;
    joystick->axes_cnt    = 0;
    joystick->buttons     = NULL;
    joystick->buttons_cnt = 0;
}

void wbInputInit(WBInput* input) {
    wbInputJoystickInit(&input->joystick, GLFW_JOYSTICK_1);
}

void wbInputJoystickUpdate(WBJoystick* joystick) {
    joystick->axes      = glfwGetJoystickAxes   (joystick->joystick_id, &joystick->axes_cnt   );
    joystick->buttons   = glfwGetJoystickButtons(joystick->joystick_id, &joystick->buttons_cnt);
    joystick->connected = joystick->axes && joystick->buttons;
}