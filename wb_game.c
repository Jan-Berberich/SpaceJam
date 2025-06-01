#define STB_IMAGE_IMPLEMENTATION
#include "wizball.h"

bool wbGameInit(WBGame* game) {
    // Initialize the GLFW library
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n"); // Print error if initialization fails
        return false;
    }

    wbWindowInit(&game->window);

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
    WBGamestate* gamestate = &game->gamestate;
    gamestate->type = WB_GAMESTATE_TITLESCREEN;
    gamestate->score = 0;
    gamestate->enemy_cnt = 0;
    gamestate->particle_cnt = 0;
    gamestate->powerup_pos = 0;
    gamestate->level = 0;
    gamestate->powerup_unlocked = WB_POWERUP_NONE;
    gamestate->powerup_permanent = WB_POWERUP_NONE;

    // Initialize players
    if (!wbPlayerWizInit(&game->player_wiz)) {
        fprintf(stderr, "Failed to initialize players\n");
        return false;
    }

    game->last_frame_time = 0;
    game->frame_cnt = 0;

    return true;
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
    WBPowerupType movement_powerup = game->gamestate.powerup_unlocked & WB_POWERUP_MAXED;
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

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

    // Draw background
    float map_view_width = WB_MAP_VIEW_WIDTH;
    float window_width = WB_WINDOW_WIDTH;
    float window_height = WB_WINDOW_HEIGHT;
    float map_height = (float)game->map.atlas.background.height / WB_MAP_CNT;
    float offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height;
    float uv_width = map_view_width / game->map.atlas.background.width;
    float offset_u = roundf(game->player_wiz.head.pos_x / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / game->map.atlas.background.width;
    float map_vertices[] = {
        //  x, y, u, v
        -1.0f + (window_width - map_view_width) / window_width, -1.0f + 2.0f * map_height / window_height + offset_y, offset_u - 0.5f * uv_width, 0.0f,
            1.0f - (window_width - map_view_width) / window_width, -1.0f + 2.0f * map_height / window_height + offset_y, offset_u + 0.5f * uv_width, 0.0f,
            1.0f - (window_width - map_view_width) / window_width, -1.0f                                     + offset_y, offset_u + 0.5f * uv_width, 1.0f / WB_MAP_CNT,
        -1.0f + (window_width - map_view_width) / window_width, -1.0f                                     + offset_y, offset_u - 0.5f * uv_width, 1.0f / WB_MAP_CNT
    };
    glBindTexture(GL_TEXTURE_2D, game->map.atlas.background.texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(map_vertices), map_vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad

    // Animate the sprite
    float sprite_atlas_width = (float)game->sprite_atlas.width;
    float sprite_atlas_height = (float)game->sprite_atlas.height;
    float sprite_size = (float)WB_SPRITE_SIZE;
    offset_y += (2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
                - 2.0f * roundf(game->player_wiz.head.pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
    offset_u = roundf(game->player_wiz.animation_angle);
    float sprite_vertices[] = {
        //  x, y, u, v
        -sprite_size / window_width, -1.0f + 2.0f * sprite_size / window_height + offset_y,  offset_u         * sprite_size / sprite_atlas_width, 0.0f,
            sprite_size / window_width, -1.0f + 2.0f * sprite_size / window_height + offset_y, (offset_u + 1.0f) * sprite_size / sprite_atlas_width, 0.0f,
            sprite_size / window_width, -1.0f                                      + offset_y, (offset_u + 1.0f) * sprite_size / sprite_atlas_width, sprite_size / sprite_atlas_height,
        -sprite_size / window_width, -1.0f                                      + offset_y,  offset_u         * sprite_size / sprite_atlas_width, sprite_size / sprite_atlas_height
    };
    glBindTexture(GL_TEXTURE_2D, game->sprite_atlas.texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite_vertices), sprite_vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad

    // Swap front and back buffers
    // The front buffer is what's currently displayed, the back buffer is what's being rendered to
    // Swapping them prevents flickering
    glfwSwapBuffers(game->window.handle);
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

int wbGameRun() {
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

        WBPowerupType movement_powerup = game.gamestate.powerup_unlocked & WB_POWERUP_MAXED;
        wbPlayerWizHandleCollision(&game.player_wiz, &game.map, movement_powerup);

        wbPlayerWizUpdate(&game.player_wiz, movement_powerup);

        wbGameRender(&game);


        game.last_frame_time = frame_time;
        game.frame_cnt++;
    }

    wbGameTerminate(&game); // Clean up resources before exiting
    return 0; // Return 0 to indicate successful execution
}
