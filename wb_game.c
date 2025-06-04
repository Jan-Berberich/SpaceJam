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
    gamestate->level = 0;
    gamestate->powerup.unlocked = WB_POWERUP_NONE;
    gamestate->powerup.permanent = WB_POWERUP_NONE;
    gamestate->powerup.slot = -1;
    gamestate->powerup.animation_colors[0] = WB_POWERUP_ANIMATION_COLOR_0;
    gamestate->powerup.animation_colors[1] = WB_POWERUP_ANIMATION_COLOR_1;
    gamestate->powerup.animation_colors[2] = WB_POWERUP_ANIMATION_COLOR_2;
    gamestate->powerup.animation_colors[3] = WB_POWERUP_ANIMATION_COLOR_3;
    gamestate->powerup.animation_colors[4] = WB_POWERUP_ANIMATION_COLOR_4;
    gamestate->powerup.animation_colors[5] = WB_POWERUP_ANIMATION_COLOR_5;

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
        enemies[i].head.type = WB_ENEMY_NONE;
    }
    game->enemy_buffer.animation_colors[0] = WB_ENEMY_ANIMATION_COLOR_0;
    game->enemy_buffer.animation_colors[1] = WB_ENEMY_ANIMATION_COLOR_1;
    game->enemy_buffer.animation_colors[2] = WB_ENEMY_ANIMATION_COLOR_2;
    game->enemy_buffer.animation_colors[3] = WB_ENEMY_ANIMATION_COLOR_3;

    // Initialize particles
    game->particle_buffer.head.cnt = 0;
    game->particle_buffer.head.type = WB_BUFFER_PARTICLE;
    WBParticle* particles = game->particle_buffer.entries;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particles[i].head.type = WB_PARTICLE_NONE;
    }

    // Initialize projectiles
    game->projectile_buffer.head.cnt = 0;
    game->projectile_buffer.head.type = WB_BUFFER_PROJECTILE;
    WBProjectile* projectiles = game->projectile_buffer.entries;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectiles[i].head.type = WB_PROJECTILE_NONE;
    }

    game->last_frame_time = 0;
    game->frame_cnt = 0;

    return true;
}

void wbGameProcessInput(WBGame* game) {
    static int prev_key_state[GLFW_KEY_LAST + 1] = {0};
    static int wiggle_cnt = 0;
    static WBDirectionType wiggle_dir = WB_DIRECTION_RIGHT;
    static double wiggle_time = 0;
    bool wiz_up = glfwGetKey(game->window.handle, WB_KEY_WIZ_UP);
    bool wiz_down = glfwGetKey(game->window.handle, WB_KEY_WIZ_DOWN);
    bool wiz_left = glfwGetKey(game->window.handle, WB_KEY_WIZ_LEFT);
    bool wiz_right = glfwGetKey(game->window.handle, WB_KEY_WIZ_RIGHT);
    bool wiz_shoot = glfwGetKey(game->window.handle, WB_KEY_WIZ_SHOOT);

    WBWiz* wiz = &game->player.wiz;

    // TODO: for debug
    bool sprint = glfwGetKey(game->window.handle, GLFW_KEY_LEFT_SHIFT);
    bool debug_strg = glfwGetKey(game->window.handle, GLFW_KEY_LEFT_CONTROL);
    if (debug_strg && !prev_key_state[GLFW_KEY_LEFT_CONTROL])
        game->gamestate.powerup.unlocked ^= WB_POWERUP_MAXED;
    prev_key_state[GLFW_KEY_LEFT_CONTROL] = debug_strg;

    double time = glfwGetTime();
    static double left_down_time;
    if (wiz_left && !prev_key_state[WB_KEY_WIZ_LEFT]) {
        left_down_time = time;
    }
    left_down_time *= wiz_left;
    static double right_down_time;
    if (wiz_right && !prev_key_state[WB_KEY_WIZ_RIGHT]) {
        right_down_time = time;
    }
    right_down_time *= wiz_right;
    if (!wiggle_cnt) {
        wiggle_time = time;
    }
    WBDirectionType dir = left_down_time > right_down_time ? WB_DIRECTION_LEFT : WB_DIRECTION_RIGHT;
    if (dir != wiggle_dir) {
        if (time - wiggle_time < WB_POWERUP_WIGGLE_TIME) {
            wiggle_dir *= -1;
            wiggle_time = time;
            wiggle_cnt++;
        }
        else {
            wiggle_cnt = 0;
        }
    }
    bool debug_f = glfwGetKey(game->window.handle, GLFW_KEY_F);
    debug_f |= wiggle_cnt >= WB_POWERUP_WIGGLE_CNT;
    if (
        debug_f && !prev_key_state[GLFW_KEY_F] &&
        (((game->gamestate.powerup.unlocked >> 2 * game->gamestate.powerup.slot) & WB_POWERUP_MAXED) < 2 || game->gamestate.powerup.slot == 4)

    ) {
        int incr = game->gamestate.powerup.slot == 2 || game->gamestate.powerup.slot == 3 || game->gamestate.powerup.slot == 6 ? 2 : 1;
        if (game->gamestate.powerup.slot == 4) {
            game->gamestate.powerup.unlocked = (game->gamestate.powerup.unlocked >> 2 * game->gamestate.powerup.slot) & WB_POWERUP_MAXED == 0 ?
                game->gamestate.powerup.unlocked + (incr << 2 * game->gamestate.powerup.slot) : 
                game->gamestate.powerup.unlocked ^ (WB_POWERUP_MAXED << 2 * game->gamestate.powerup.slot);

        }
        else if (game->gamestate.powerup.slot == 5) {
            //activate bomb
        }
        else {
            game->gamestate.powerup.unlocked += incr << 2 * game->gamestate.powerup.slot;
        }
        game->gamestate.powerup.slot = -1;
    }
    prev_key_state[GLFW_KEY_F] = debug_f;

    // wbPlayerWizProcessInput
    if (wiz_right) {
        wiz->vel_x_key += WB_PLAYER_WIZ_ACC_X;
        wiz->vel_x_key = fminf(wiz->vel_x_key,   WB_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint);
    }
    if (wiz_left) {
        wiz->vel_x_key -= WB_PLAYER_WIZ_ACC_X;
        wiz->vel_x_key = fmaxf(wiz->vel_x_key, -(WB_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint));
    }

    float vel_x, vel_y;
    if (game->gamestate.powerup.unlocked & WB_POWERUP_ANTIGRAV) {
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

    if (wiz_shoot && !prev_key_state[WB_KEY_WIZ_SHOOT] && wiz->onscreen_bullet_cnt < WB_PLAYER_WIZ_ONSCREEN_BULLET_CNT_MAX) {
        wiz->next_bullet_direction = game->gamestate.powerup.unlocked & WB_POWERUP_DOUBLE ? -wiz->next_bullet_direction :
            fsgnf(wiz->vel_x_key) ? fsgnf(wiz->vel_x_key) : wiz->next_bullet_direction;
        float projectile_vel_x = (float)wiz->next_bullet_direction * WB_PROJECTILE_VEL;
        float projectile_vel_y = 0.0f;
        wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_BULLET, wiz->pos_x, wiz->pos_y, projectile_vel_x, projectile_vel_y);
    }
    prev_key_state[WB_KEY_WIZ_SHOOT] = wiz_shoot;
    prev_key_state[WB_KEY_WIZ_LEFT] = wiz_left;
    prev_key_state[WB_KEY_WIZ_RIGHT] = wiz_right;
}

void wbGameDraw(GLuint texture_id, GLuint vbo,
    float width_x, float offset_x, float height_y, float offset_y, float width_u, float offset_u, float height_v, float offset_v) {
    static float vertices[16];

    //           x                                   y                                                  u                                  v
    vertices[ 0] = -width_x + offset_x; vertices[ 1] = -1.0f + 2.0f * height_y + offset_y; vertices[ 2] = offset_u          ; vertices[ 3] = offset_v           ;
    vertices[ 4] =  width_x + offset_x; vertices[ 5] = -1.0f + 2.0f * height_y + offset_y; vertices[ 6] = width_u + offset_u; vertices[ 7] = offset_v           ;
    vertices[ 8] =  width_x + offset_x; vertices[ 9] = -1.0f                   + offset_y; vertices[10] = width_u + offset_u; vertices[11] = offset_v + height_v;
    vertices[12] = -width_x + offset_x; vertices[13] = -1.0f                   + offset_y; vertices[14] =           offset_u; vertices[15] = offset_v + height_v;
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw quad
}

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

    // Set the key color
    GLint keyColorLoc = glGetUniformLocation(game->shader.program, "keyColor");
    glUniform4f(keyColorLoc, 1.0f, 0.0f, 1.0f, 1.0f); // #FF00FFFF
    // Set the replacement color later
    GLint replaceColorLoc = glGetUniformLocation(game->shader.program, "replaceColor");

    // Sprite height is centered in atlas. They get shifted up by half a pixel with + 2.0 in offset_y

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
    float offset_u = map->view_center_x / map->atlas.background.width - 0.5f * width_u;
    float height_v = 1.0f / WB_MAP_CNT;
    float offset_v = (float)game->gamestate.level / WB_MAP_CNT;
    wbGameDraw(map->atlas.background.texture_id, game->shader.vbo,
        width_x, 0.0f, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );

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
        switch ((WBProjectileType)projectile->head.type) {
            case WB_PROJECTILE_NONE:
            continue;

            case WB_PROJECTILE_BULLET:
            offset_u = (float)WB_PROJECTILE_BULLET_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = (float)WB_PROJECTILE_BULLET_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((projectile->head.pos_x - map->view_center_x) / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(projectile->head.pos_y / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_height;
        wbGameDraw(game->sprite_atlas.texture_id, game->shader.vbo,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // Draw Particles
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &game->particle_buffer.entries[i];
        switch ((WBParticleType)particle->head.type) {
            case WB_PARTICLE_NONE:
            continue;

            case WB_PARTICLE_POWERUP:
            offset_u = (float)WB_PARTICLE_POWERUP_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = (float)WB_PARTICLE_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_PARTICLE_DECAY:
            offset_u = (float)WB_PARTICLE_DECAY_SPRITE_ATLAS_X / sprite_atlas_width + (float)(
                (uint64_t)((double)particle->frame_age * WB_PARTICLE_DECAY_ANIMATION_SPEED)
                % WB_PARTICLE_DECAY_ANIMATION_FRAME_CNT
            ) * sprite_size / sprite_atlas_width;
            offset_v = (float)WB_PARTICLE_DECAY_SPRITE_ATLAS_Y / sprite_atlas_width;
            uint32_t rgba = game->enemy_buffer.animation_colors[(int)particle->head.color_key];
            float r = (float)((rgba >> 24) & 0xFF) / 0xFF;
            float g = (float)((rgba >> 16) & 0xFF) / 0xFF;
            float b = (float)((rgba >>  8) & 0xFF) / 0xFF;
            float a = (float)((rgba >>  0) & 0xFF) / 0xFF;
            glUniform4f(replaceColorLoc, r, g, b, a); // #FF00FFFF
            break;
        }
        offset_x = 2.0f * roundf((particle->head.pos_x - map->view_center_x) / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(particle->head.pos_y / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_height;
        wbGameDraw(game->sprite_atlas.texture_id, game->shader.vbo,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // Draw Enemies
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &game->enemy_buffer.entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERBLUE:
            offset_u = (float)WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_X / sprite_atlas_width + (float)(
                (uint64_t)((double)enemy->frame_age * WB_ENEMY_SPINNERBLUE_ANIMATION_SPEED)
                % WB_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT
            ) * sprite_size / sprite_atlas_width;
            offset_v = (float)WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_ENEMY_CIRCLE:
            offset_u = (float)WB_ENEMY_CIRCLE_SPRITE_ATLAS_X / sprite_atlas_width + (float)(
                (uint64_t)((double)enemy->frame_age * WB_ENEMY_CIRCLE_ANIMATION_SPEED)
                % WB_ENEMY_CIRCLE_ANIMATION_FRAME_CNT
            ) * sprite_size / sprite_atlas_width;
            offset_v = (float)WB_ENEMY_CIRCLE_SPRITE_ATLAS_Y / sprite_atlas_height;
            uint32_t rgba = game->enemy_buffer.animation_colors[(int)enemy->head.color_key];
            float r = (float)((rgba >> 24) & 0xFF) / 0xFF;
            float g = (float)((rgba >> 16) & 0xFF) / 0xFF;
            float b = (float)((rgba >>  8) & 0xFF) / 0xFF;
            float a = (float)((rgba >>  0) & 0xFF) / 0xFF;
            glUniform4f(replaceColorLoc, r, g, b, a); // #FF00FFFF
            break;
        }
        offset_x = 2.0f * roundf((enemy->head.pos_x - map->view_center_x) / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(enemy->head.pos_y / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_height;
        wbGameDraw(game->sprite_atlas.texture_id, game->shader.vbo,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // Draw Player Wiz
    offset_x = 2.0f * roundf((wiz->pos_x - map->view_center_x) / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos_y / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT / window_height;
    offset_u = roundf(wiz->animation_angle) * width_u + WB_PLAYER_WIZ_SPRITE_ATLAS_X / sprite_atlas_width;
    offset_v = (float)WB_PLAYER_WIZ_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(game->sprite_atlas.texture_id, game->shader.vbo,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );

    // Draw Powerup Slots
    for (int i = 0; i < WB_POWERUP_SLOT_CNT; i++) {
        if (i == game->gamestate.powerup.slot) {
            uint32_t rgba = game->gamestate.powerup.animation_colors[(uint64_t)((float)game->frame_cnt * WB_POWERUP_ANIMATION_COLOR_SPEED) % WB_POWERUP_ANIMATION_COLOR_CNT];
            float r = (float)((rgba >> 24) & 0xFF) / 0xFF;
            float g = (float)((rgba >> 16) & 0xFF) / 0xFF;
            float b = (float)((rgba >>  8) & 0xFF) / 0xFF;
            float a = (float)((rgba >>  0) & 0xFF) / 0xFF;
            glUniform4f(replaceColorLoc, r, g, b, a); // #FF00FFFF
        }
        else {
            glUniform4f(replaceColorLoc, (float)0x6A / 0xFF, (float)0x65 / 0xFF, (float)0xEE / 0xFF, 1.0f); // #6A65EEFF
        }
        offset_x = -map_view_width / window_width + WB_SPRITE_SIZE / window_width;
        offset_y = 2.0f * WB_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size - 14.0f) / window_height
                 - 1.5f * i * WB_SPRITE_SIZE / window_height;
        int powerup_slot_state = ((game->gamestate.powerup.unlocked >> 2 * i) & WB_POWERUP_MAXED);
        if (i == 4) {
            powerup_slot_state &= 1;
        }
        if (powerup_slot_state < 2) {
            offset_u = ((2.0f * i + powerup_slot_state) * WB_SPRITE_SIZE + WB_POWERUP_SPRITE_ATLAS_X) / sprite_atlas_width;
        }
        else {
            offset_u = (float)WB_POWERUP_MAXED_SPRITE_ATLAS_X / sprite_atlas_width;
        }
        offset_v = (float)WB_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
        wbGameDraw(game->sprite_atlas.texture_id, game->shader.vbo,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

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
            uint32_t seed = game.frame_cnt * 4;
            wbBufferAppend(&game.enemy_buffer, WB_ENEMY_SPINNERBLUE, randfin(seed, 200, 3000), randfin(seed + 1, 50, 200));
            wbBufferAppend(&game.enemy_buffer, WB_ENEMY_CIRCLE, randfin(seed + 2, 200, 3000), randfin(seed + 3, 50, 200));
        }

        wbWindowLockAspectRatio(&game.window);

        wbGameProcessInput(&game);

        wbPlayerWizHandleCollision(&game.player.wiz, &game.map, game.gamestate.powerup.unlocked);

        wbPlayerWizUpdate(&game.player.wiz, game.gamestate.powerup.unlocked);
        game.map.view_center_x = roundf(game.player.wiz.pos_x / WB_MAP_SUBPIXEL_CNT) * WB_MAP_SUBPIXEL_CNT;
        game.map.view_center_x = fmaxf(game.map.view_center_x, WB_MAP_VIEW_WIDTH / 2);
        game.map.view_center_x = fminf(game.map.view_center_x, game.map.atlas.background.width - WB_MAP_VIEW_WIDTH / 2 + 1);
        wbParticleUpdate(&game.particle_buffer, &game.player.wiz, &game.gamestate.powerup.slot);
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
