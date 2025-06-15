#include "wizball.h"

bool wbWindowInit(WBWindow* window) {
    // Configure GLFW to use OpenGL 3.3 Core Profile
    // These hints are important for modern OpenGL development
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Request OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

    // Create a windowed mode window and its OpenGL context
    // Parameters: width, height, title, monitor (for fullscreen), share (for context sharing)
    window->handle = glfwCreateWindow(WB_WINDOW_WIDTH, WB_WINDOW_HEIGHT, "Wizball", NULL, NULL);
    if (!window->handle) {
        fprintf(stderr, "Failed to create GLFW window\n"); // Print error if window creation fails
        glfwTerminate(); // Terminate GLFW before exiting
        return false;
    }

    // Make the window's context current
    // This makes the OpenGL context associated with 'window' the current one on the calling thread
    glfwMakeContextCurrent(window->handle);

    // GLAD Initialization
    // Load all OpenGL function pointers through GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate(); // Terminate GLFW before exiting
        return false;
    }

    // Set up Viewport and Enable Blending (for transparency in PNGs)
    glfwGetFramebufferSize(window->handle, &window->width, &window->height);
    glViewport(0, 0, window->width, window->height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending
}

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