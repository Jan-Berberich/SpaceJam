#include "wb.h"

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float fsgnf(float f) {
    return f != 0.0f ? -2.0f * signbit(f) + 1.0f : 0.0f;
}

// --- Shader Sources ---
const char* vertex_shader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec2 pos;\n"
    "layout (location = 1) in vec2 tex;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(pos, 0.0, 1.0);\n"
    "    TexCoord = tex;\n"
    "}\n";

const char* fragment_shader_src =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D texSampler;\n"
    "void main() {\n"
    "    FragColor = texture(texSampler, TexCoord);\n"
    "}\n";

// --- Helper function to check shader compilation/linking errors ---
void wbCheckShaderError(GLuint shader, const char* type) {
    GLint success;
    GLchar info_log[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, info_log);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, info_log);
        }
    }
}

// --- Function to compile and link shaders ---
GLuint wbCreateShader(const char* vertex_source, const char* fragment_source) {
    // Vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    wbCheckShaderError(vertex, "VERTEX");

    // Fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    wbCheckShaderError(fragment, "FRAGMENT");

    // Shader Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    wbCheckShaderError(program, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
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

void wbTextureInit(WBTexture* texture, uint8_t* data, int width, int height) {
    texture->width = width;
    texture->height = height;

    glGenTextures(1, &texture->texture_id); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, texture->texture_id); // Bind the texture
    // Upload image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for scaling

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge to avoid border artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // filtering for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // filtering for magnification
}

bool wbMapInit(WBMap* map) {
    WBMapAtlas* map_atlas = &map->atlas;
    int width, height, channel_cnt;
    uint8_t* data = stbi_load("sprite/map_background_atlas.png", &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->background, data, width, height);
    stbi_image_free(data);

    data = stbi_load("sprite/map_foreground_atlas.png", &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map foreground atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->foreground, data, width, height);
    map_atlas->collider = malloc(width * height * (sizeof *map_atlas->collider));
    if (!map_atlas->collider) {
        fprintf(stderr, "Failed to allocate memory for foreground collider\n");
        stbi_image_free(data);
        return false;
    }
    for (int i = 0; i < width * height; ++i) {
        map_atlas->collider[i] = !!data[i * 4 + 3]; // Alpha channel
    }
    stbi_image_free(data);

    map->atlas_y = 0;
    map->type = WB_MAP_0;
    map->collider = map_atlas->collider;

    return true;
}

float wbGetCollisionAngle(WBPlayerWiz* wiz, WBMap* map) {
    WBEntityHeader* wizh = &wiz->head;
    int xc = wizh->pos_x;
    int yc = wizh->pos_y;
    bool collision;
    int collision_cnt = 0;
    float collision_x = 0.0f;
    float collision_y = 0.0f;

    // Check directions around the player for collision (circle approximation)
    float* angles = wiz->collider_angles;
    int x; int y;
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        x = xc + (int)roundf(WB_PLAYER_WIZ_COLLISION_RADIUS * cosf(angles[i]));
        y = yc + (int)roundf(WB_PLAYER_WIZ_COLLISION_RADIUS * sinf(angles[i]));
        collision = map->collider[y * map->atlas.foreground.width + x];
        collision_cnt += collision;
        collision_x += collision * (x - xc);
        collision_y += collision * (y - yc);
    }

    return atan2f(collision_y / collision_cnt, collision_x / collision_cnt);
}

bool wbPlayerWizInit(WBPlayerWiz* wiz) {
    wiz->collider_angles = malloc(WB_PLAYER_WIZ_COLLISION_ANGLE_CNT * (sizeof *wiz->collider_angles));
    if (!wiz->collider_angles) {
        fprintf(stderr, "Failed to allocate memory for collider_angles");
        return false;
    }
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        wiz->collider_angles[i] = (float)i * M_2PI / WB_PLAYER_WIZ_COLLISION_ANGLE_CNT;
    }

    WBEntityHeader* entity = &wiz->head;
    WBSprite* sprite = &entity->sprite;

    sprite->atlas_x = WB_PLAYER_WIZ_SPRITE_ATLAS_X;
    sprite->atlas_y = WB_PLAYER_WIZ_SPRITE_ATLAS_Y;
    entity->health = WB_PLAYER_WIZ_HEALTH_MAX;

    sprite->type = WB_SPRITE_PLAYER;
    entity->pos_x = WB_WINDOW_WIDTH / 2;
    entity->pos_y = WB_PLAYER_WIZ_INIT_POS_Y;
    wiz->vel_x_key = 0.0f;
    wiz->vel_y = 0.0f;

    wiz->vel_x_values[0] = WB_PLAYER_WIZ_VEL_X_0;
    wiz->vel_x_values[1] = WB_PLAYER_WIZ_VEL_X_1;
    wiz->vel_x_values[2] = WB_PLAYER_WIZ_VEL_X_2;
    wiz->vel_x_values[3] = WB_PLAYER_WIZ_VEL_X_3;
    wiz->vel_x_values[4] = WB_PLAYER_WIZ_VEL_X_4;
    wiz->vel_x_values[5] = WB_PLAYER_WIZ_VEL_X_5;
    wiz->vel_x_values[6] = WB_PLAYER_WIZ_VEL_X_6;
    wiz->animation_speed_values[0] = WB_PLAYER_WIZ_ANIMATION_SPEED_0;
    wiz->animation_speed_values[1] = WB_PLAYER_WIZ_ANIMATION_SPEED_1;
    wiz->animation_speed_values[2] = WB_PLAYER_WIZ_ANIMATION_SPEED_2;
    wiz->animation_speed_values[3] = WB_PLAYER_WIZ_ANIMATION_SPEED_3;
    wiz->animation_speed_values[4] = WB_PLAYER_WIZ_ANIMATION_SPEED_4;
    wiz->animation_speed_values[5] = WB_PLAYER_WIZ_ANIMATION_SPEED_5;
    wiz->animation_speed_values[6] = WB_PLAYER_WIZ_ANIMATION_SPEED_6;

    wiz->vel_y_values[0] = WB_PLAYER_WIZ_VEL_Y_0;
    wiz->vel_y_values[1] = WB_PLAYER_WIZ_VEL_Y_1;
    wiz->vel_y_values[2] = WB_PLAYER_WIZ_VEL_Y_2;
    wiz->vel_y_values[3] = WB_PLAYER_WIZ_VEL_Y_3;
    wiz->vel_y_values[4] = WB_PLAYER_WIZ_VEL_Y_4;

    wiz->animation_angle = 0.0f;

    return true;
}

void wbShaderInit(WBShader* shader) {
    // Create and Compile Shaders
    shader->program = wbCreateShader(vertex_shader_src, fragment_shader_src);
    glUseProgram(shader->program); // Use the shader program

    shader->indices[0] = 0; shader->indices[1] = 1; shader->indices[2] = 2; // first triangle
    shader->indices[3] = 2; shader->indices[4] = 3; shader->indices[5] = 0; // second triangle

    glGenVertexArrays(1, &shader->vao);
    glGenBuffers(1, &shader->vbo);
    glGenBuffers(1, &shader->ebo);

    glBindVertexArray(shader->vao);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shader->vertices), shader->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shader->indices), shader->indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texcoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void wbGamestateInit(WBGamestate* gamestate) {
    gamestate->type = WB_GAMESTATE_TITLESCREEN;
    gamestate->score = 0;
    gamestate->enemy_cnt = 0;
    gamestate->particle_cnt = 0;
    gamestate->powerup_pos = 0;
    gamestate->level = 0;
    gamestate->powerup_unlocked = WB_POWERUP_NONE;
    gamestate->powerup_permanent = WB_POWERUP_NONE;
}

bool wbGameInit(WBGame* game) {
    // Initialize the GLFW library
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n"); // Print error if initialization fails
        return false;
    }

    // Configure GLFW to use OpenGL 3.3 Core Profile
    // These hints are important for modern OpenGL development
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Request OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

    // Create a windowed mode window and its OpenGL context
    // Parameters: width, height, title, monitor (for fullscreen), share (for context sharing)
    game->window.handle = glfwCreateWindow(WB_WINDOW_WIDTH, WB_WINDOW_HEIGHT, "Wizzball", NULL, NULL);
    if (!game->window.handle) {
        fprintf(stderr, "Failed to create GLFW window\n"); // Print error if window creation fails
        glfwTerminate(); // Terminate GLFW before exiting
        return false;
    }

    // Make the window's context current
    // This makes the OpenGL context associated with 'window' the current one on the calling thread
    glfwMakeContextCurrent(game->window.handle);

    // GLAD Initialization
    // Load all OpenGL function pointers through GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate(); // Terminate GLFW before exiting
        return false;
    }

    // Set up Viewport and Enable Blending (for transparency in PNGs)
    glfwGetFramebufferSize(game->window.handle, &game->window.width, &game->window.height);
    glViewport(0, 0, game->window.width, game->window.height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

    // Initialize the shader program
    wbShaderInit(&game->shader);

    // Initialize sprite atlas
    int width, height, channel_cnt;
    uint8_t* data = stbi_load("sprite/sprite_atlas.png", &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load sprite atlas image\n");
        return false;
    }
    wbTextureInit(&game->sprite_atlas, data, width, height);
    stbi_image_free(data);

    // Initialize map
    if (!wbMapInit(&game->map)) {
        fprintf(stderr, "Failed to initialize map\n");
        return false;
    }

    // Initialize game state
    wbGamestateInit(&game->gamestate);

    // Initialize players
    if (!wbPlayerWizInit(&game->player_wiz)) {
        fprintf(stderr, "Failed to initialize players\n");
        return false;
    }

    game->last_frame_time = 0;
    game->frame_cnt = 0;

    return true;
}

void wbGameTerminate(WBGame* game) {
    // Cleanup resources
    glDeleteVertexArrays(1, &game->shader.vao);
    glDeleteBuffers(1, &game->shader.vbo);
    glDeleteBuffers(1, &game->shader.ebo);
    glDeleteProgram(game->shader.program);
    glDeleteTextures(1, &game->sprite_atlas.texture_id); // Delete the sprite atlas texture
    free(game->player_wiz.collider_angles); // Free the player collider angles memory
    free(game->map.atlas.collider); // Free the map atlas collider memory
    glfwDestroyWindow(game->window.handle); // Destroy the window
    glfwTerminate(); // Terminate GLFW to clean up resources
}

void wbGameProcessInput(WBGame* game) {
    static int prev_key_state[GLFW_KEY_LAST + 1] = {0};
    bool wiz_up = glfwGetKey(game->window.handle, WB_KEY_WIZ_UP);
    bool wiz_down = glfwGetKey(game->window.handle, WB_KEY_WIZ_DOWN);
    bool wiz_left = glfwGetKey(game->window.handle, WB_KEY_WIZ_LEFT);
    bool wiz_right = glfwGetKey(game->window.handle, WB_KEY_WIZ_RIGHT);
    bool wiz_shoot = glfwGetKey(game->window.handle, WB_KEY_WIZ_SHOOT);

    WBPlayerWiz* wiz = &game->player_wiz;

    // TODO: for debug
    if (wiz_shoot && !prev_key_state[WB_KEY_WIZ_SHOOT])
        game->gamestate.powerup_unlocked = (game->gamestate.powerup_unlocked + 1) % 3;

    if (wiz_right) {
        wiz->vel_x_key += WB_PLAYER_WIZ_ACC_X;
        wiz->vel_x_key = fminf(wiz->vel_x_key, WB_PLAYER_WIZ_VEL_X_CNT - 1);
    }
    if (wiz_left) {
        wiz->vel_x_key -= WB_PLAYER_WIZ_ACC_X;
        wiz->vel_x_key = fmaxf(wiz->vel_x_key, -WB_PLAYER_WIZ_VEL_X_CNT + 1);
    }

    float vel_x, vel_y;
    WBPowerup movement_powerup = game->gamestate.powerup_unlocked & WB_POWERUP_MAXED;
    switch(movement_powerup) {
        case WB_POWERUP_NONE: {
            break;
        }
        case WB_POWERUP_THRUST: {
            break;
        }
        case WB_POWERUP_ANTIGRAV: {
            if (wiz_down) {
                wiz->vel_y_key += WB_PLAYER_WIZ_ACC_Y;
                wiz->vel_y_key = fminf(wiz->vel_y_key, WB_PLAYER_WIZ_VEL_Y_CNT - 1);
            }
            if (wiz_up) {
                wiz->vel_y_key -= WB_PLAYER_WIZ_ACC_Y;
                wiz->vel_y_key = fmaxf(wiz->vel_y_key, -WB_PLAYER_WIZ_VEL_Y_CNT + 1);
            }

            vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
            wiz->vel_x_key -= fsgnf(vel_x) * WB_PLAYER_WIZ_DEC_X * (!wiz_left && !wiz_right);
            vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
            wiz->vel_y_key -= fsgnf(vel_y) * WB_PLAYER_WIZ_DEC_Y * (!wiz_up && !wiz_down);
            break;
        }
    }
    prev_key_state[WB_KEY_WIZ_SHOOT] = wiz_shoot;
}

int wbRun() {
    WBGame game;
    if(!wbGameInit(&game)) {
        fprintf(stderr, "Failed to initialize game\n");
        glfwTerminate(); // Terminate GLFW to clean up resources
        return -1; // Return -1 to indicate failure
    }

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(game.window.handle)) {
        unsigned long long frame_time = (unsigned long long)round(glfwGetTime() * WB_FPS);

        // Poll for and process events
        // This checks for input events (keyboard, mouse) and window events (resize, close)
        glfwPollEvents();

        // Run with fps
        if (frame_time == game.last_frame_time) continue;

        wbWindowLockAspectRatio(&game.window);

        wbGameProcessInput(&game);

        // handle collision
        WBPlayerWiz* wiz = &game.player_wiz;
        WBPowerup movement_powerup = game.gamestate.powerup_unlocked & WB_POWERUP_MAXED;
        int pos_y = roundf(game.player_wiz.head.pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT;
        bool map_ceil_collision = pos_y - WB_PLAYER_WIZ_COLLISION_RADIUS < WB_MAP_CEIL_HEIGHT;
        bool map_floor_collision = pos_y + WB_PLAYER_WIZ_COLLISION_RADIUS > WB_MAP_FLOOR_HEIGHT;
        float collision_angle;
        if (map_ceil_collision || map_floor_collision) {
            collision_angle = M_2PI / 4.0f * (map_floor_collision ? 1.0f : -1.0f);
        }
        else {
            collision_angle = wbGetCollisionAngle(&game.player_wiz, &game.map);
        }

        if (!isnan(collision_angle)) {
            float collision_x = cosf(collision_angle);
            float collision_y = sinf(collision_angle);
            if (fabsf(collision_x) > WB_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN) {
                wiz->vel_x_key = -fabsf(wiz->vel_x_key) * fsgnf(collision_x);
            }
            if (fabsf(collision_y) > 0.01f) {
                if (movement_powerup == WB_POWERUP_NONE || movement_powerup == WB_POWERUP_THRUST) {
                    wiz->vel_y += WB_PLAYER_WIZ_GRAVITY;
                    wiz->vel_y = -fabsf(wiz->vel_y) * fsgnf(collision_y);
                }
                else {
                    if (map_ceil_collision || map_floor_collision) {
                        wiz->vel_y_key = -fsgnf(collision_y);
                        wiz->vel_y_key += WB_PLAYER_WIZ_ACC_Y * fsgnf(collision_y);
                    }
                    else {
                        wiz->vel_y_key -= WB_PLAYER_WIZ_ACC_Y * fsgnf(collision_y);
                        wiz->vel_y_key = -fabsf(wiz->vel_y_key) * fsgnf(collision_y);
                    }
                }
            }
        }

        // update state
        if (movement_powerup == WB_POWERUP_NONE && !isnan(collision_angle) || movement_powerup != WB_POWERUP_NONE) {
            wiz->vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))] * WB_VELOCITY_FACTOR;
        }
        if (movement_powerup == WB_POWERUP_ANTIGRAV) {
            wiz->vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))] * WB_VELOCITY_FACTOR;
        }
        if (movement_powerup == WB_POWERUP_NONE || movement_powerup == WB_POWERUP_THRUST) {
            wiz->vel_y += WB_PLAYER_WIZ_GRAVITY;
        }
        wiz->head.pos_x += wiz->vel_x;
        wiz->head.pos_y += wiz->vel_y;

        wiz->animation_angle += fsgnf(wiz->vel_x_key) * wiz->animation_speed_values[(int)roundf(fabsf(wiz->vel_x_key))];
        wiz->animation_angle += wiz->animation_angle <  -0.5f ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
        wiz->animation_angle -= wiz->animation_angle >= -0.5f + WB_PLAYER_WIZ_ANIMATION_FRAME_CNT ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
        
        game.last_frame_time = frame_time;
        game.frame_cnt++;



        // Render
        // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
        glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

        // Draw background
        float map_view_width = WB_MAP_VIEW_WIDTH;
        float window_width = WB_WINDOW_WIDTH;
        float window_height = WB_WINDOW_HEIGHT;
        float map_height = (float)game.map.atlas.background.height / WB_MAP_CNT;
        float offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height;
        float uv_width = map_view_width / game.map.atlas.background.width;
        float offset_u = roundf(game.player_wiz.head.pos_x / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / game.map.atlas.background.width;
        float map_vertices[] = {
            //  x, y, u, v
            -1.0f + (window_width - map_view_width) / window_width, -1.0f + 2.0f * map_height / window_height + offset_y, offset_u - 0.5f * uv_width, 0.0f,
             1.0f - (window_width - map_view_width) / window_width, -1.0f + 2.0f * map_height / window_height + offset_y, offset_u + 0.5f * uv_width, 0.0f,
             1.0f - (window_width - map_view_width) / window_width, -1.0f                                     + offset_y, offset_u + 0.5f * uv_width, 1.0f / WB_MAP_CNT,
            -1.0f + (window_width - map_view_width) / window_width, -1.0f                                     + offset_y, offset_u - 0.5f * uv_width, 1.0f / WB_MAP_CNT
        };
        glBindTexture(GL_TEXTURE_2D, game.map.atlas.background.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, game.shader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(map_vertices), map_vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad

        // Animate the sprite
        float sprite_atlas_width = (float)game.sprite_atlas.width;
        float sprite_atlas_height = (float)game.sprite_atlas.height;
        float sprite_size = (float)WB_SPRITE_SIZE;
        offset_y += (2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
                   - 2.0f * roundf(game.player_wiz.head.pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
        offset_u = roundf(game.player_wiz.animation_angle);
        float sprite_vertices[] = {
            //  x, y, u, v
            -sprite_size / window_width, -1.0f + 2.0f * sprite_size / window_height + offset_y,  offset_u         * sprite_size / sprite_atlas_width, 0.0f,
             sprite_size / window_width, -1.0f + 2.0f * sprite_size / window_height + offset_y, (offset_u + 1.0f) * sprite_size / sprite_atlas_width, 0.0f,
             sprite_size / window_width, -1.0f                                      + offset_y, (offset_u + 1.0f) * sprite_size / sprite_atlas_width, sprite_size / sprite_atlas_height,
            -sprite_size / window_width, -1.0f                                      + offset_y,  offset_u         * sprite_size / sprite_atlas_width, sprite_size / sprite_atlas_height
        };
        glBindTexture(GL_TEXTURE_2D, game.sprite_atlas.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, game.shader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite_vertices), sprite_vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad

        // Swap front and back buffers
        // The front buffer is what's currently displayed, the back buffer is what's being rendered to
        // Swapping them prevents flickering
        glfwSwapBuffers(game.window.handle);
    }

    wbGameTerminate(&game); // Clean up resources before exiting
    return 0; // Return 0 to indicate successful execution
}
