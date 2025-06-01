#include "wb.h"

void wbWindowLockAspectRatio(WBWindow* window) {
    glfwGetWindowSize(window->handle, &window->width, &window->height);

    // Aspect ratio lock
    static float target_aspect = (float)WB_WINDOW_WIDTH / (float)WB_WINDOW_HEIGHT;
    float window_aspect = (float)window->width / (float)window->height;
    int viewport_x = 0, viewport_y = 0, viewport_w = window->width, viewport_h = window->height;

    if (window_aspect > target_aspect) {
        // Window is wider than target: pillarbox
        viewport_w = (int)roundf((float)window->height * target_aspect);
        viewport_x = (window->width - viewport_w) / 2;
        viewport_y = 0;
        viewport_h = window->height;
    } else {
        // Window is taller than target: letterbox
        viewport_h = (int)roundf((float)window->width / target_aspect);
        viewport_y = (window->height - viewport_h) / 2;
        viewport_x = 0;
        viewport_w = window->width;
    }
    glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
}