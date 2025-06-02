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
    uint8_t* data = stbi_load(WB_SPRITE_ATLAS_PATH, &width, &height, &channel_cnt, 4);
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
    gamestate->state = WB_GAMESTATE_TITLESCREEN;
    gamestate->lifes = 3;
    gamestate->score = 0;
    gamestate->powerup_slot = 0;
    gamestate->level = 0;
    gamestate->powerup_unlocked = WB_POWERUP_NONE;
    gamestate->powerup_permanent = WB_POWERUP_NONE;

    // Initialize players
    int pos_x_min = WB_MAP_VIEW_WIDTH / 2 - 1;
    int pos_x_max = game->map.atlas.background.width - WB_MAP_VIEW_WIDTH / 2 + 1;
    if (!wbPlayerWizInit(&game->player.wiz, pos_x_min, pos_x_max)) {
        fprintf(stderr, "Failed to initialize players\n");
        return false;
    }

    // Initialize enemies
    game->enemy_buffer.head.cnt = 0;
    game->enemy_buffer.head.type = WB_BUFFER_ENEMY;
    WBEnemy* enemies = game->enemy_buffer.entries;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemies[i].type = WB_ENEMY_NONE;
    }

    // Initialize particles
    game->particle_buffer.head.cnt = 0;
    game->particle_buffer.head.type = WB_BUFFER_PARTICLE;
    WBParticle* particles = game->particle_buffer.entries;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particles[i].type = WB_PARTICLE_NONE;
    }

    // Initialize projectiles
    game->projectile_buffer.head.cnt = 0;
    game->projectile_buffer.head.type = WB_BUFFER_PROJECTILE;
    WBProjectile* projectiles = game->projectile_buffer.entries;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectiles[i].type = WB_PROJECTILE_NONE;
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

    WBWiz* wiz = &game->player.wiz;

    // TODO: for debug
    bool debug_shift = glfwGetKey(game->window.handle, GLFW_KEY_LEFT_SHIFT);
    if (debug_shift && !prev_key_state[GLFW_KEY_LEFT_SHIFT])
        game->gamestate.powerup_unlocked = (game->gamestate.powerup_unlocked + 1) % 3;
    prev_key_state[GLFW_KEY_LEFT_SHIFT] = debug_shift;

    // wbPlayerWizProcessInput
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
    if (movement_powerup == WB_POWERUP_ANTIGRAV) {
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
    }

    wiz->next_bullet_direction = game->gamestate.powerup_unlocked & WB_POWERUP_DOUBLE ? -wiz->next_bullet_direction :
        fsgnf(wiz->vel_x_key) ? fsgnf(wiz->vel_x_key) : wiz->next_bullet_direction;
    if (wiz_shoot && !prev_key_state[WB_KEY_WIZ_SHOOT] && wiz->onscreen_bullet_cnt < WB_PLAYER_WIZ_ONSCREEN_BULLET_CNT_MAX) {
        float projectile_vel_x = (float)wiz->next_bullet_direction * WB_PROJECTILE_VEL;
        float projectile_vel_y = 0.0f;
        wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_BULLET, wiz->pos_x, wiz->pos_y, projectile_vel_x, projectile_vel_y);
    }
    prev_key_state[WB_KEY_WIZ_SHOOT] = wiz_shoot;
}

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

    WBMap* map = &game->map;
    WBWiz* wiz = &game->player.wiz;
    WBProjectile* projectile;
    WBParticle* particle;
    WBEnemy* enemy;

    // Draw background
    float map_view_width = WB_MAP_VIEW_WIDTH;
    float window_width = WB_WINDOW_WIDTH;
    float window_height = WB_WINDOW_HEIGHT;
    float map_height = (float)map->atlas.background.height / WB_MAP_CNT;
    float width_x = map_view_width / window_width;
    float height_y = map_height / window_height;
    float offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height;
    float width_u = map_view_width / map->atlas.background.width;
    float offset_u = map->view_center_x / map->atlas.background.width;
    float height_v = 1.0f / WB_MAP_CNT;
    float offset_v = game->gamestate.level / WB_MAP_CNT;
    float map_vertices[] = {
        //  x, y, u, v
        -width_x, -1.0f + 2.0f * height_y + offset_y, offset_u - 0.5f * width_u, offset_v,
         width_x, -1.0f + 2.0f * height_y + offset_y, offset_u + 0.5f * width_u, offset_v,
         width_x, -1.0f                   + offset_y, offset_u + 0.5f * width_u, offset_v + height_v,
        -width_x, -1.0f                   + offset_y, offset_u - 0.5f * width_u, offset_v + height_v
    };
    glBindTexture(GL_TEXTURE_2D, map->atlas.background.texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(map_vertices), map_vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad

    // Draw Projectiles
    float offset_x;
    float sprite_atlas_width = (float)game->sprite_atlas.width;
    float sprite_atlas_height = (float)game->sprite_atlas.height;
    float sprite_size = (float)WB_SPRITE_SIZE;
    width_x = sprite_size / window_width;
    height_y = sprite_size / window_height;
    width_u = sprite_size / sprite_atlas_width;
    height_v = sprite_size / sprite_atlas_height;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &game->projectile_buffer.entries[i];
        switch (projectile->type) {
            case WB_PROJECTILE_NONE:
            continue;

            case WB_PROJECTILE_BULLET:
            offset_u = WB_PROJECTILE_BULLET_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_PROJECTILE_BULLET_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((projectile->pos_x - map->view_center_x) / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
                 - 2.0f * roundf(projectile->pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
        float sprite_vertices[] = {
            //  x, y, u, v
            -width_x + offset_x, -1.0f + 2.0f * height_y + offset_y,           offset_u, offset_v,
             width_x + offset_x, -1.0f + 2.0f * height_y + offset_y, width_u + offset_u, offset_v,
             width_x + offset_x, -1.0f                   + offset_y, width_u + offset_u, offset_v + height_v,
            -width_x + offset_x, -1.0f                   + offset_y,           offset_u, offset_v + height_v
        };
        glBindTexture(GL_TEXTURE_2D, game->sprite_atlas.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite_vertices), sprite_vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad
    }

    // Draw Particles
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &game->particle_buffer.entries[i];
        switch (particle->type) {
            case WB_PARTICLE_NONE:
            continue;

            case WB_PARTICLE_POWERUP:
            offset_u = WB_PARTICLE_POWERUP_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_PARTICLE_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((particle->pos_x - map->view_center_x) / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
                 - 2.0f * roundf(particle->pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
        float sprite_vertices[] = {
            //  x, y, u, v
            -width_x + offset_x, -1.0f + 2.0f * height_y + offset_y,           offset_u, offset_v,
             width_x + offset_x, -1.0f + 2.0f * height_y + offset_y, width_u + offset_u, offset_v,
             width_x + offset_x, -1.0f                   + offset_y, width_u + offset_u, offset_v + height_v,
            -width_x + offset_x, -1.0f                   + offset_y,           offset_u, offset_v + height_v
        };
        glBindTexture(GL_TEXTURE_2D, game->sprite_atlas.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite_vertices), sprite_vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad
    }

    // Draw Enemies
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &game->enemy_buffer.entries[i];
        switch (enemy->type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERBLUE:
            offset_u = WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_X / sprite_atlas_width
                     + roundf(enemy->animation_frame) * WB_SPRITE_SIZE / sprite_atlas_width;
            offset_v = WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((enemy->pos_x - map->view_center_x) / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
                 - 2.0f * roundf(enemy->pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
        float sprite_vertices[] = {
            //  x, y, u, v
            -width_x + offset_x, -1.0f + 2.0f * height_y + offset_y,           offset_u, offset_v,
             width_x + offset_x, -1.0f + 2.0f * height_y + offset_y, width_u + offset_u, offset_v,
             width_x + offset_x, -1.0f                   + offset_y, width_u + offset_u, offset_v + height_v,
            -width_x + offset_x, -1.0f                   + offset_y,           offset_u, offset_v + height_v
        };
        glBindTexture(GL_TEXTURE_2D, game->sprite_atlas.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, game->shader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprite_vertices), sprite_vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad
    }

    // Draw Player Wiz
    offset_x = 2.0f * roundf((wiz->pos_x - map->view_center_x) / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT / window_width;
    offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - WB_SPRITE_SIZE + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT / window_height;
    width_u = sprite_size / sprite_atlas_width;
    offset_u = roundf(wiz->animation_angle) * width_u + WB_PLAYER_WIZ_SPRITE_ATLAS_X / sprite_atlas_width;
    height_v = sprite_size / sprite_atlas_height;
    offset_v = WB_PLAYER_WIZ_SPRITE_ATLAS_X / sprite_atlas_height;
    float sprite_vertices[] = {
        //  x, y, u, v
        -width_x + offset_x, -1.0f + 2.0f * height_y + offset_y,           offset_u, offset_v,
         width_x + offset_x, -1.0f + 2.0f * height_y + offset_y, width_u + offset_u, offset_v,
         width_x + offset_x, -1.0f                   + offset_y, width_u + offset_u, offset_v + height_v,
        -width_x + offset_x, -1.0f                   + offset_y,           offset_u, offset_v + height_v
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
    free(game->player.wiz.collider_angles); // Free the player collider angles memory
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
        uint64_t frame_time = (uint64_t)round(glfwGetTime() * WB_FPS);

        // Poll for and process events
        // This checks for input events (keyboard, mouse) and window events (resize, close)
        glfwPollEvents();

        // Run with fps
        if (frame_time == game.last_frame_time) continue;

        // TODO: for debug
        if (game.enemy_buffer.head.cnt < 8) {
            wbBufferAppend(&game.enemy_buffer, WB_ENEMY_SPINNERBLUE, randfin(game.frame_cnt, 200, 3000), randfin(game.frame_cnt++, 50, 200));
        }

        wbWindowLockAspectRatio(&game.window);

        wbGameProcessInput(&game);

        WBPowerupType movement_powerup = game.gamestate.powerup_unlocked & WB_POWERUP_MAXED;
        wbPlayerWizHandleCollision(&game.player.wiz, &game.map, movement_powerup);

        wbPlayerWizUpdate(&game.player.wiz, movement_powerup);
        game.map.view_center_x = roundf(game.player.wiz.pos_x / WB_SUBPIXEL_X_CNT) * WB_SUBPIXEL_X_CNT;
        game.map.view_center_x = fmaxf(game.map.view_center_x, WB_MAP_VIEW_WIDTH / 2);
        game.map.view_center_x = fminf(game.map.view_center_x, game.map.atlas.background.width - WB_MAP_VIEW_WIDTH / 2 + 1);
        wbParticleUpdate(&game.particle_buffer, &game.player.wiz, &game.gamestate.powerup_slot);
        wbEnemyUpdate(&game.enemy_buffer, &game.player.wiz, &game.particle_buffer);
        wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer);

        // TODO: for debug
        if (game.player.wiz.health <= 0) {
            int pos_x_min = WB_MAP_VIEW_WIDTH / 2 - 1;
            int pos_x_max = game.map.atlas.background.width - WB_MAP_VIEW_WIDTH / 2 + 1;
            wbPlayerWizInit(&game.player.wiz, pos_x_min, pos_x_max);
        }

        wbGameRender(&game);


        game.last_frame_time = frame_time;
        game.frame_cnt++;
    }

    wbGameTerminate(&game); // Clean up resources before exiting
    return 0; // Return 0 to indicate successful execution
}
