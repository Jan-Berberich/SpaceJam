#include "wizball.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef WB_NO_SOUND
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#endif

bool wbGameInit(WBGame* game) {
    // Initialize the GLFW library
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n"); // Print error if initialization fails
        return false;
    }

    // Initialize the window
    wbWindowInit(&game->window);
    
    // Initialize the shader program
    wbShaderInit(&game->shader);
    
    // Initialize graphic
    if (!wbGraphicInit(&game->graphic, &game->map)) {
        fprintf(stderr, "Failed to initialize graphic\n");
        wbShaderUninit(&game->shader);
        glfwDestroyWindow(game->window.handle);
        glfwTerminate();
        return false;
    }

    // Initialize sound
    if (!wbSoundInit(&game->sound)) {
        fprintf(stderr, "Failed to initialize sound\n");
        wbGraphicUninit(&game->graphic, &game->map);
        wbShaderUninit(&game->shader);
        glfwDestroyWindow(game->window.handle);
        glfwTerminate();
        return false;
    }


    
    // Initialize gamestate
    game->gamestate.state = -1; // avoid reading uninitialized memory
    wbGamestateSetupTitlescreen(&game->gamestate, &game->sound);

    // Initialize enemies
    game->enemy_buffer.head.cnt = 0;
    game->enemy_buffer.head.type = WB_BUFFER_ENEMY;

    // Initialize particles
    game->particle_buffer.head.cnt = 0;
    game->particle_buffer.head.type = WB_BUFFER_PARTICLE;

    // Initialize projectiles
    game->projectile_buffer.head.cnt = 0;
    game->projectile_buffer.head.type = WB_BUFFER_PROJECTILE;

    return true;
}

void wbGameUninit(WBGame* game) {
    // Cleanup resources
    wbSoundUninit(&game->sound);
    wbGraphicUninit(&game->graphic, &game->map);
    wbShaderUninit(&game->shader);
    glfwDestroyWindow(game->window.handle);
    glfwTerminate();
}

void wbGameProcessInput(WBGame* game) {
    static double input_time = 1.0 / WB_GAMERULE_PROCESS_INPUT_SPEED;
    static double timer = 1.0 / WB_GAMERULE_PROCESS_INPUT_SPEED;
    if (timer < input_time) {
        timer += game->gamestate.delta_time;
        return;
    }
    timer += game->gamestate.delta_time;
    timer -= input_time;

    WBView* view = &game->map.view;
    
    static int wiggle_cnt = 0;
    static double wiggle_time = 0;
    static WBDirectionType wiggle_dir = WB_DIRECTION_POSITIVE;
    bool wiz_up = glfwGetKey(game->window.handle, WB_KEY_WIZ_UP);
    bool wiz_down = glfwGetKey(game->window.handle, WB_KEY_WIZ_DOWN);
    bool wiz_left = glfwGetKey(game->window.handle, WB_KEY_WIZ_LEFT);
    bool wiz_right = glfwGetKey(game->window.handle, WB_KEY_WIZ_RIGHT);
    bool wiz_fire = glfwGetKey(game->window.handle, WB_KEY_WIZ_FIRE);
    bool cat_up = glfwGetKey(game->window.handle, WB_KEY_CAT_UP);
    bool cat_down = glfwGetKey(game->window.handle, WB_KEY_CAT_DOWN);
    bool cat_left = glfwGetKey(game->window.handle, WB_KEY_CAT_LEFT);
    bool cat_right = glfwGetKey(game->window.handle, WB_KEY_CAT_RIGHT);
    bool cat_fire = glfwGetKey(game->window.handle, WB_KEY_CAT_FIRE);

    WBWiz* wiz = &game->player.wiz;

    // wbGamestatePowerupProcessInput
    WBPowerup* powerup = &game->gamestate.powerup;
    bool sprint = glfwGetKey(game->window.handle, WB_KEY_SPRINT); // TODO: not in real game
    bool debug_strg = glfwGetKey(game->window.handle, WB_KEY_TOGGLEGRAV);
    if (debug_strg && !game->window.prev_key_state[WB_KEY_TOGGLEGRAV])
        powerup->unlocked ^= WB_POWERUP_SLOTMASK;
    game->window.prev_key_state[WB_KEY_TOGGLEGRAV] = debug_strg;

    static double left_down_time;
    if (wiz_left && !game->window.prev_key_state[WB_KEY_WIZ_LEFT]) {
        left_down_time = game->gamestate.time;
    }
    left_down_time *= wiz_left;
    static double right_down_time;
    if (wiz_right && !game->window.prev_key_state[WB_KEY_WIZ_RIGHT]) {
        right_down_time = game->gamestate.time;
    }
    right_down_time *= wiz_right;
    if (!wiggle_cnt) {
        wiggle_time = game->gamestate.time;
    }
    WBDirectionType dir = left_down_time > 0 || right_down_time > 0 ?
                          left_down_time > right_down_time ? WB_DIRECTION_NEGATIVE : WB_DIRECTION_POSITIVE : 0;
    if (dir != 0 && dir != wiggle_dir) {
        if (game->gamestate.time - wiggle_time < 1.0f / WB_GAMERULE_POWERUP_WIGGLE_SPEED) {
            wiggle_dir *= -1;
            wiggle_time = game->gamestate.time;
            wiggle_cnt++;
        }
        else {
            wiggle_cnt = 0;
        }
    }
    bool activate_powerup = glfwGetKey(game->window.handle, WB_KEY_POWERUP);
    activate_powerup |= wiggle_cnt >= WB_GAMERULE_POWERUP_WIGGLE_CNT;
    activate_powerup &= game->gamestate.state == WB_GAMESTATE_PLAY;
    int powerup_slotstate = (powerup->unlocked >> 2 * powerup->slot) & WB_POWERUP_SLOTMASK;
    if (activate_powerup && !game->window.prev_key_state[WB_KEY_POWERUP] && powerup->slot >= 0 &&
        (powerup_slotstate < WB_POWERUP_SLOTMASK - 1 || powerup->slot == (int)(0.5f * log2f(WB_POWERUP_WIZSPRAY + 1)))) {
        
        ma_sound_seek_to_pcm_frame(&game->sound.powerup_activate, 0);
        ma_sound_start(&game->sound.powerup_activate);

        int incr = powerup->slot == (int)(0.5f * log2f(WB_POWERUP_CAT + 1)) ||
                   powerup->slot == (int)(0.5f * log2f(WB_POWERUP_BLAZERS + 1))||
                   powerup->slot == (int)(0.5f * log2f(WB_POWERUP_SHIELD + 1)) ?
                   2 : 1;
        if (powerup->slot == (int)(0.5f * log2f(WB_POWERUP_WIZSPRAY + 1))) {
            powerup->unlocked = powerup_slotstate == 0 ?
                                powerup->unlocked + (incr << 2 * powerup->slot) : 
                                powerup->unlocked ^ (WB_POWERUP_SLOTMASK << 2 * powerup->slot);

        }
        else if (powerup->slot == (int)(0.5f * log2f(WB_POWERUP_BOMB + 1))) {
            //TODO: activate bomb
            ma_sound_seek_to_pcm_frame(&game->sound.bomb, 0);
            ma_sound_start(&game->sound.bomb);
        }
        else {
            powerup->unlocked += incr << 2 * powerup->slot;
        }
        if (powerup->unlocked & WB_POWERUP_BEAM && powerup->slot == (int)(0.5f * log2f(WB_POWERUP_BEAM + 1))) {
            int idx = wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_BEAM, &wiz->pos, &wiz->vel);
            game->projectile_buffer.entries[idx].head.animation_key = WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT - 1;
        }
        powerup->slot = -1;
    }
    game->window.prev_key_state[WB_KEY_POWERUP] = activate_powerup;

    // wbPlayerWizProcessInput
    static double wiz_fire_down_time;
    if (wiz_fire && !game->window.prev_key_state[WB_KEY_WIZ_FIRE]) {
        wiz_fire_down_time = game->gamestate.time;
    }
    wiz_fire_down_time *= wiz_fire;
    bool autofire = wiz_fire_down_time > 0 &&
                    game->gamestate.time - wiz_fire_down_time >= WB_GAMERULE_AUTOFIRE_HOLD_TIME &&
                    powerup->unlocked & WB_POWERUP_CAT;
    if (autofire) {
        cat_right |= wiz_right;
        cat_left  |= wiz_left;
        cat_up    |= wiz_up;
        cat_down  |= wiz_down;
        wiz_right  = false;
        wiz_left   = false;
        wiz_up     = false;
        wiz_down   = false;
    }
    if (wiz_right) {
        wiz->vel_x_key += WB_GAMERULE_PLAYER_WIZ_ACC_X * input_time;
        wiz->vel_x_key = fminf(wiz->vel_x_key,   WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint);
    }
    if (wiz_left) {
        wiz->vel_x_key -= WB_GAMERULE_PLAYER_WIZ_ACC_X * input_time;
        wiz->vel_x_key = fmaxf(wiz->vel_x_key, -(WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint));
    }

    float vel_x, vel_y;
    if (powerup->unlocked & WB_POWERUP_ANTIGRAV) {
        if (wiz_down) {
            wiz->vel_y_key += WB_GAMERULE_PLAYER_WIZ_ACC_Y * input_time;
            wiz->vel_y_key = fminf(wiz->vel_y_key,  WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT - 1);
        }
        if (wiz_up) {
            wiz->vel_y_key -= WB_GAMERULE_PLAYER_WIZ_ACC_Y * input_time;
            wiz->vel_y_key = fmaxf(wiz->vel_y_key, -WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT + 1);
        }
        vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
        wiz->vel_x_key -= fsgnf(vel_x) * WB_GAMERULE_PLAYER_WIZ_DEC_X * input_time * (!wiz_left && !wiz_right);
        vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
        wiz->vel_y_key -= fsgnf(vel_y) * WB_GAMERULE_PLAYER_WIZ_DEC_Y * input_time * (!wiz_up && !wiz_down);
    }

    static double last_wiz_fire_time = 0.0;
    static double last_cat_fire_time = 0.0;
    if ((wiz_fire && !game->window.prev_key_state[WB_KEY_WIZ_FIRE] || autofire) && 
        (game->gamestate.state == WB_GAMESTATE_PLAY || game->gamestate.state == WB_GAMESTATE_SPAWN)) {
        WBVec2f vel;
        if (view->bullet_wiz_cnt < WB_GAMERULE_VIEW_BULLET_WIZ_CNT_MAX) {
            if (game->gamestate.time - last_wiz_fire_time <  1.05 * input_time &&
                game->gamestate.time - last_cat_fire_time >= 1.05 * input_time) {
                //ma_sound_seek_to_pcm_frame(&game->sound.fire_spam, 0); // TODO: remove fire_spam?
                //ma_sound_start(&game->sound.fire_spam);
            } else {
                ma_sound_seek_to_pcm_frame(&game->sound.fire, 0);
                ma_sound_start(&game->sound.fire);
            }
            vel.x = powerup->unlocked & WB_POWERUP_DOUBLE ?
                WB_GAMERULE_PROJECTILE_VEL * wiz->next_bullet_direction : WB_GAMERULE_PROJECTILE_VEL * wiz->facing;
            vel.y = 0.0f;
            WBProjectileType type = powerup->unlocked & WB_POWERUP_BLAZERS ? WB_PROJECTILE_BLAZER_WIZ : WB_PROJECTILE_BULLET_WIZ;
            wbProjectileAppend(&game->projectile_buffer, type, &wiz->pos, &vel);
            wiz->next_bullet_direction *= -1;
            last_wiz_fire_time = game->gamestate.time;
        }

        if (!view->spray && powerup->unlocked & WB_POWERUP_WIZSPRAY) {
            if (wiz->next_spray_direction == WB_DIRECTION_NEGATIVE) {
                vel.y = -WB_GAMERULE_PROJECTILE_VEL;
                vel.x = -WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NW, &wiz->pos, &vel);
                vel.x = 0;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_N, &wiz->pos, &vel);
                vel.x = WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NE, &wiz->pos, &vel);
            }
            else {
                vel.y =  WB_GAMERULE_PROJECTILE_VEL;
                vel.x = -WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NE, &wiz->pos, &vel);
                vel.x = 0;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_N, &wiz->pos, &vel);
                vel.x = WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NW, &wiz->pos, &vel);
            }
            wiz->next_spray_direction *= -1;
            view->spray = true;
        }

        if (!view->beam && powerup->unlocked & (WB_POWERUP_BEAM | WB_POWERUP_DOUBLE)) {
            int idx = wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_BEAM, &wiz->pos, &vel);
            game->projectile_buffer.entries[idx].head.color_key = randfin(time(NULL), 0, WB_GRAPHIC_PROJECTILE_BEAM_COLORMAP_CNT);
        }
    }
    game->window.prev_key_state[WB_KEY_WIZ_LEFT] = wiz_left;
    game->window.prev_key_state[WB_KEY_WIZ_RIGHT] = wiz_right;

    // wbPlayerCatProcessInput
    WBCat* cat = &game->player.cat;
    if (!(powerup->unlocked & WB_POWERUP_CAT)) {
        game->window.prev_key_state[WB_KEY_WIZ_FIRE] = wiz_fire;
        return;
    }

    cat->vel.x = 0.0f;
    cat->vel.y = 0.0f;
    cat->vel.x += WB_GAMERULE_PLAYER_CAT_VEL * cat_right;
    cat->vel.x -= WB_GAMERULE_PLAYER_CAT_VEL * cat_left;
    cat->vel.y += WB_GAMERULE_PLAYER_CAT_VEL * cat_down;
    cat->vel.y -= WB_GAMERULE_PLAYER_CAT_VEL * cat_up;

    cat->hold_position = cat_right || cat_left || cat_up || cat_down;

    if (cat_fire && !game->window.prev_key_state[WB_KEY_CAT_FIRE] ||
        wiz_fire && !game->window.prev_key_state[WB_KEY_WIZ_FIRE] || autofire) {
        WBVec2f vel;
        if (view->bullet_cat_cnt < WB_GAMERULE_VIEW_BULLET_CAT_CNT_MAX) {
            if (game->gamestate.time - last_cat_fire_time <  1.05 * input_time &&
                game->gamestate.time - last_wiz_fire_time >= 1.05 * input_time) {
                //ma_sound_seek_to_pcm_frame(&game->sound.fire_spam, 0);
                //ma_sound_start(&game->sound.fire_spam);
            } else {
                ma_sound_seek_to_pcm_frame(&game->sound.fire, 0);
                ma_sound_start(&game->sound.fire);
            }
            vel.x = WB_GAMERULE_PROJECTILE_VEL * cat->facing;
            vel.y = 0.0f;
            WBProjectileType type = powerup->unlocked & WB_POWERUP_BLAZERS ? WB_PROJECTILE_BLAZER_CAT : WB_PROJECTILE_BULLET_CAT;
            wbProjectileAppend(&game->projectile_buffer, type, &cat->pos, &vel);
            last_cat_fire_time = game->gamestate.time;
        }
        if (!view->spray && powerup->unlocked & WB_POWERUP_CATSPRAY) {
            if (cat->next_spray_direction == WB_DIRECTION_NEGATIVE) {
                vel.y = -WB_GAMERULE_PROJECTILE_VEL;
                vel.x = -WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NW, &cat->pos, &vel);
                vel.x = 0;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_N, &cat->pos, &vel);
                vel.x = WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NE, &cat->pos, &vel);
            }
            else {
                vel.y =  WB_GAMERULE_PROJECTILE_VEL;
                vel.x = -WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NE, &cat->pos, &vel);
                vel.x = 0;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_N, &cat->pos, &vel);
                vel.x = WB_GAMERULE_PROJECTILE_VEL;
                wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_SPRAY_NW, &cat->pos, &vel);
            }
            cat->next_spray_direction *= -1;
            view->spray = true;
        }
    }
    cat->pos_y_buffer[cat->pos_y_buffer_idx] = wiz->pos.y;
    cat->pos_y_buffer_idx = (cat->pos_y_buffer_idx + 1) % WB_GAMERULE_PLAYER_CAT_MOVEDELAY_INPUT_FRAME_CNT;

    game->window.prev_key_state[WB_KEY_WIZ_FIRE] = wiz_fire;
    game->window.prev_key_state[WB_KEY_CAT_FIRE] = cat_fire;
}

void wbGameDrawBatchClear(WBShader* shader) {
    shader->batch_cnt = 0;
}

void wbGameDrawBatchAppend(WBShader* shader,
    float width_x, float offset_x, float height_y, float offset_y, float width_u, float offset_u, float height_v, float offset_v) {
    int vbase = shader->batch_cnt * WB_GRAPHIC_SPRITE_VERTICES_CNT * 4;

    // x, y, u, v
    shader->vertices[vbase +  0] = -width_x + offset_x; shader->vertices[vbase +  1] = -1.0f + 2.0f * height_y + offset_y; shader->vertices[vbase +  2] = offset_u          ; shader->vertices[vbase +  3] = offset_v           ;
    shader->vertices[vbase +  4] =  width_x + offset_x; shader->vertices[vbase +  5] = -1.0f + 2.0f * height_y + offset_y; shader->vertices[vbase +  6] = width_u + offset_u; shader->vertices[vbase +  7] = offset_v           ;
    shader->vertices[vbase +  8] =  width_x + offset_x; shader->vertices[vbase +  9] = -1.0f                   + offset_y; shader->vertices[vbase + 10] = width_u + offset_u; shader->vertices[vbase + 11] = offset_v + height_v;
    shader->vertices[vbase + 12] = -width_x + offset_x; shader->vertices[vbase + 13] = -1.0f                   + offset_y; shader->vertices[vbase + 14] =           offset_u; shader->vertices[vbase + 15] = offset_v + height_v;

    shader->batch_cnt++;
}

void wbGameDrawBatch(WBShader* shader, GLuint texture_id) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof *shader->vertices) * WB_GRAPHIC_SPRITE_VERTICES_CNT * 4 * shader->batch_cnt, shader->vertices, GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, WB_GRAPHIC_SPRITE_INDICES_CNT * shader->batch_cnt, GL_UNSIGNED_INT, 0);
}

void wbGameDraw(WBShader* shader, GLuint texture_id,
    float width_x, float offset_x, float height_y, float offset_y, float width_u, float offset_u, float height_v, float offset_v) {
    
    wbGameDrawBatchClear(shader);
    wbGameDrawBatchAppend(shader, width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
    wbGameDrawBatch(shader, texture_id);
}

void wbGameDrawText(WBGame* game, char* text, WBTextType text_type, float width_scale, float height_scale, double draw_time,
    float offset_x, float offset_y,
    uint32_t* colors, int color_cnt, float color_speed, WBColorMode color_mode) {
    
    if (game->gamestate.time < draw_time) return;
    
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;

    static float replace_colors[WB_GRAPHIC_COLORMAP_ALL32_CNT * 4];
    if (color_mode == 1) {
        int cnt = (game->gamestate.time - draw_time) * color_speed / WB_GRAPHIC_TEXT_COLORBAND_HEIGHT + 1;
        cnt = cnt > color_cnt ? color_cnt : cnt;
        for (int i = 0; i < color_cnt; i++) {
            uint32_t color = i >= cnt ? 0x000000FF : colors[i];
            int idx = (-i + color_cnt) % color_cnt;
            ui32to4f(&replace_colors[idx * 4], color);
        }
        glUniform1f(game->shader.loc.replace_color_speed, color_speed);
        glUniform1i(game->shader.loc.replace_color_cnt, color_cnt);
    } else {
        int idx = (uint64_t)((game->gamestate.time - draw_time) * color_speed) % color_cnt;
        ui32to4f(replace_colors, colors[idx]);
        color_cnt = 1;
    }
    glUniform1i(game->shader.loc.key_color_mode, color_mode);
    glUniform4fv(game->shader.loc.replace_colors, color_cnt, replace_colors);
    int i;
    float string_width;
    float _offset_x, offset_u;
    wbGameDrawBatchClear(&game->shader);
    switch (text_type) {
        case WB_TEXT_DIGIT:
        string_width = strlen(text);
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width * width_scale;
        for (char* p = text; *p; p++) {
            if (*p != ' ') {
                if ('0' <= *p && *p <= '9') {
                    i = *p - '0' + 26;
                }
                else if ('A' <= *p && *p <= 'Z') {
                    i = *p - 'A';
                } else {
                    i = WB_GRAPHIC_TEXT_DIGIT_DOT_SPRITE_ATLAS_OFFSET;
                }
                offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_height * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y / sprite_atlas_height);
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width * width_scale;
        }
        break;

        case WB_TEXT_SMALL:
        string_width = strlen(text);
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width;
        for (char* p = text; *p; p++) {
            if (*p != ' ') {
                i = *p == '-' ? WB_GRAPHIC_TEXT_SMALL_MINUS_SPRITE_ATLAS_OFFSET : *p - 'a';
                offset_u = (WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_height * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_Y / sprite_atlas_height);
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width;
        }
        break;

        case WB_TEXT_LARGE:
        string_width = 0.0f;
        for (char* p = text; *p; p++) {
            string_width += *p == 'I' || *p == 'L'? 0.5f : 1.0f;
        }
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width;
        for (char* p = text; *p; p++) {
            float char_w = *p == 'I' || *p == 'L' ? 0.5f : 1.0f;
            if (*p != ' ') {
                i = *p == '!' ? 26 : *p - 'A';
                offset_u = (WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_height * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_Y / sprite_atlas_height);
            }
            _offset_x += 2.0f * char_w * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width;
        }
        break;

        case WB_TEXT_TITLE:
        wbGameDrawBatchAppend(&game->shader,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH / window_width  * width_scale,  offset_x,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / window_height * height_scale, offset_y,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH  / sprite_atlas_width,  WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_X / sprite_atlas_width,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / sprite_atlas_height, WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_Y / sprite_atlas_height);
        break;
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);
}
        
void wbGameDrawDust(WBGame* game) {
    WBGraphic* graphic = &game->graphic;
    WBView* view = &game->map.view;
    float view_width = WB_GRAPHIC_MAP_VIEW_WIDTH;
    float map_height = (float)graphic->background_atlas.height / WB_MAP_CNT;
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float dust_sprite_size = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE;
    float dust_sprite_width = dust_sprite_size * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X;
    float dust_sprite_height = dust_sprite_size * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y;
    float width_x = dust_sprite_width / window_width;
    float offset_x;
    float height_y = dust_sprite_height / window_height;
    float offset_y;
    float width_u = dust_sprite_size / graphic->dust_texture.width;
    float offset_u;
    float height_v = dust_sprite_size / graphic->dust_texture.height;
    float offset_v = 0.0f;
    int dust_row_cnt = ceil(WB_GAMERULE_MAP_HORIZON_HEIGHT / dust_sprite_height);
    int dust_col_cnt = ceil(WB_GRAPHIC_MAP_VIEW_WIDTH / dust_sprite_width) + 1;
    wbGameDrawBatchClear(&game->shader);
    for (int layer = 0; layer < WB_GRAPHIC_MAP_DUST_LAYER_CNT; layer++) {
        for (int row = 0; row < dust_row_cnt; row++) {
            for (int col = 0; col < dust_col_cnt; col++) {
                offset_x = (-2.0f * roundf(view->center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT
                            -2.0f * roundf(WB_GRAPHIC_MAP_DUST_VELOCITY_FACTOR * layer * view->center_x / WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT)
                           / window_width + width_x + 2.0f * col * width_x;
                offset_x = fmodf(offset_x, 2.0f * dust_col_cnt * width_x);
                offset_x += (offset_x < -dust_col_cnt * width_x) ? 2.0f * dust_col_cnt * width_x : 0.0f;
                offset_x += (row / (dust_row_cnt - 1.0f) - 0.5f) * width_x;
                offset_y = 2.0f * (WB_GRAPHIC_MAP_VIEW_OFFSET_Y + map_height) / window_height - 2.0f * height_y - 2.0f * row * height_y;
                offset_u = layer * dust_sprite_size / graphic->dust_texture.width;
                wbGameDrawBatchAppend(&game->shader,
                    width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
            }
        }
        if (layer == 0) {
            wbGameDrawBatch(&game->shader, graphic->dust_texture.texture_id);
            wbGameDrawBatchClear(&game->shader);
            glUniform1f(game->shader.loc.key_alpha, 128.0/255.0);
        }
    }
    wbGameDrawBatch(&game->shader, graphic->dust_texture.texture_id);
    glUniform1f(game->shader.loc.key_alpha, -1.0);
}

void wbGameDrawMap(WBGame* game, WBTexture* texture) {
    WBView* view = &game->map.view;
    WBGraphic* graphic = &game->graphic;
    float view_width = WB_GRAPHIC_MAP_VIEW_WIDTH;
    float map_height = (float)texture->height / WB_MAP_CNT;
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float width_x = view_width / window_width;
    float offset_x = 0.0f;
    float height_y = map_height / window_height;
    float offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height;
    float width_u = view_width / texture->width;
    float offset_u = roundf(view->center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT / texture->width - 0.5f * width_u;
    float height_v = 1.0f / WB_MAP_CNT;
    float offset_v = (float)game->gamestate.level / WB_MAP_CNT;
    wbGameDraw(&game->shader, texture->texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
}

void wbGameDrawEntities(WBGame* game) {
    WBProjectile* projectile;
    WBParticle* particle;
    WBEnemy* enemy;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->graphic.background_atlas.height / WB_MAP_CNT;
    float rgba[4];
    uint32_t color;
    uint32_t prev_color;

    // particles
    wbGameDrawBatchClear(&game->shader);
    prev_color = game->graphic.colormap.enemy[(int)game->particle_buffer.entries->head.color_key + WB_GRAPHIC_ENEMY_COLORMAP_OFFSET];
    ui32to4f(rgba, prev_color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &game->particle_buffer.entries[i];
        switch ((WBParticleType)particle->head.type) {
            case WB_PARTICLE_NONE:
            continue;

            case WB_PARTICLE_POWERUP:
            offset_u = WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_PARTICLE_DECAY:
            offset_u = WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_X / sprite_atlas_width + (int)particle->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_Y / sprite_atlas_width;
            break;

            case WB_PARTICLE_DROPLET_FALL:
            offset_u = WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_X / sprite_atlas_width + (int)particle->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_Y / sprite_atlas_width;
            break;
            
            case WB_PARTICLE_DROPLET_SPLAT:
            offset_u = WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_X / sprite_atlas_width + (int)particle->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_Y / sprite_atlas_width;
            break;
        }
        offset_x = 2.0f * roundf((particle->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(particle->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        color = game->graphic.colormap.enemy[(int)particle->head.color_key + WB_GRAPHIC_ENEMY_COLORMAP_OFFSET];
        if (color != prev_color) {
            wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);
            wbGameDrawBatchClear(&game->shader);
            ui32to4f(rgba, color);
            glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
        }
        wbGameDrawBatchAppend(&game->shader,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
        prev_color = color;
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);

    // enemies
    wbGameDrawBatchClear(&game->shader);
    prev_color = game->graphic.colormap.enemy[(int)game->enemy_buffer.entries->head.color_key + WB_GRAPHIC_ENEMY_COLORMAP_OFFSET];
    ui32to4f(rgba, prev_color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &game->enemy_buffer.entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERCYAN:
            offset_u = WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_X / sprite_atlas_width + (int)enemy->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_ENEMY_CIRCLE:
            offset_u = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_ENEMY_DROPLET:
            offset_u = WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_X / sprite_atlas_width + (int)enemy->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((enemy->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(enemy->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        color = game->graphic.colormap.enemy[(int)enemy->head.color_key + WB_GRAPHIC_ENEMY_COLORMAP_OFFSET];
        if (color != prev_color) {
            wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);
            wbGameDrawBatchClear(&game->shader);
            ui32to4f(rgba, color);
            glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
        }
        wbGameDrawBatchAppend(&game->shader,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
        prev_color = color;
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);

    // projectiles
    wbGameDrawBatchClear(&game->shader);
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &game->projectile_buffer.entries[i];
        switch (projectile->head.type) {
            case WB_PROJECTILE_NONE: continue;
            case WB_PROJECTILE_BULLET_WIZ:
            offset_u = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_BULLET_CAT:
            offset_u = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_BLAZER_WIZ:
            offset_u = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_BLAZER_CAT:
            offset_u = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_SPRAY_NW:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_SPRAY_N:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_SPRAY_NE:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
            case WB_PROJECTILE_BEAM:
                int color_key = (int)projectile->head.color_key % 2 ? projectile->head.color_key / 4 + 1 : 0;
                uint32_t color = game->graphic.colormap.beam[color_key];
                ui32to4f(rgba, color);
                glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
                projectile->head.pos.y -= 0.5f * WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
                offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                         +(2.0f * map_height - sprite_size + 2.0f) / window_height
                         - 2.0f * roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / sprite_atlas_width + (int)projectile->head.animation_key * width_u;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / sprite_atlas_height;
                wbGameDrawBatchAppend(&game->shader,
                    width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
                projectile->head.pos.y += WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / sprite_atlas_width + ((int)projectile->head.animation_key + WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT) * width_u;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        wbGameDrawBatchAppend(&game->shader,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);
}

void wbGameDrawPlayerCat(WBGame* game) {
    WBCat* cat = &game->player.cat;
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->graphic.background_atlas.height / WB_MAP_CNT;

    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width
             + 2.0f * roundf((cat->pos.x - wiz->pos.x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(cat->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_X / sprite_atlas_width
             + (uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_CAT_ANIMATION_SPEED + 0.5) % WB_GRAPHIC_PLAYER_CAT_ANIMATION_FRAME_CNT * width_u;
    offset_v = WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
}

void wbGameDrawPlayerWiz(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->graphic.background_atlas.height / WB_MAP_CNT;

    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = roundf(wiz->animation_angle) * width_u + WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_X / sprite_atlas_width;
    offset_v = WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
}

void wbGameDrawPlayerWizSpawn(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->graphic.background_atlas.height / WB_MAP_CNT;

    // player wiz
    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_X / sprite_atlas_width
             + ((uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) % (game->graphic.sprite_atlas.width / WB_GRAPHIC_SPRITE_SIZE)) * width_u;
    offset_v = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_Y / sprite_atlas_height
             + ((uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) / (game->graphic.sprite_atlas.width / WB_GRAPHIC_SPRITE_SIZE)) * height_v;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
}

void wbGameDrawGui(WBGame* game) {
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->graphic.sprite_atlas.width;
    float sprite_atlas_height = game->graphic.sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float rgba[4];

    // powerup slots
    wbGameDrawBatchClear(&game->shader);
    int i = 0;
    int powerup_slotstate;
    offset_y = 2.0f - 2.0f * WB_GRAPHIC_GUI_POWERUP_STRIDE / window_height;
    offset_v = WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
    uint32_t color = game->graphic.colormap.blue6[1];
    ui32to4f(rgba, color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    for (; i < WB_POWERUP_SLOT_CNT; i++) {
        if (i == game->gamestate.powerup.slot) continue;
        offset_x = -1.0f + (window_width - (WB_POWERUP_SLOT_CNT - 1) * WB_GRAPHIC_GUI_POWERUP_STRIDE) / window_width + 2.0f * i * WB_GRAPHIC_GUI_POWERUP_STRIDE / window_width;
        powerup_slotstate = ((game->gamestate.powerup.unlocked >> 2 * i) & WB_POWERUP_SLOTMASK);
        if (i == 4) powerup_slotstate &= 1;
        offset_u = powerup_slotstate < 2 ?
            ((2.0f * i + powerup_slotstate) * sprite_size + WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X) / sprite_atlas_width :
            WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X / sprite_atlas_width;
        wbGameDrawBatchAppend(&game->shader,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas.texture_id);
    i = game->gamestate.powerup.slot;
    color = game->graphic.colormap.blue6[(uint64_t)(game->gamestate.time * WB_GRAPHIC_GUI_COLORMAP_SPEED + 0.5) % WB_GRAPHIC_COLORMAP_BLUE6_CNT];
    ui32to4f(rgba, color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    offset_x = -1.0f + (window_width - (WB_POWERUP_SLOT_CNT - 1) * WB_GRAPHIC_GUI_POWERUP_STRIDE) / window_width + 2.0f * i * WB_GRAPHIC_GUI_POWERUP_STRIDE / window_width;
    powerup_slotstate = ((game->gamestate.powerup.unlocked >> 2 * i) & WB_POWERUP_SLOTMASK);
    if (i == 4) powerup_slotstate &= 1;
    offset_u = powerup_slotstate < 2 ?
        ((2.0f * i + powerup_slotstate) * sprite_size + WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X) / sprite_atlas_width :
        WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X / sprite_atlas_width;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v);

    // 1UP
    offset_y = 2.0f - 2.0f * WB_GRAPHIC_GUI_SCORE_OFFSET_Y / window_height;
    wbGameDrawText(game, "   1UP", WB_TEXT_DIGIT, 1.0f, 1.0f, 0,
        -1.0f, offset_y,
        game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // score
    offset_y -= 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_height;;
    sprintf(game->graphic.text, "      %06i", game->gamestate.score);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 0,
        -1.0f, offset_y,
        game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // lifes
    sprintf(game->graphic.text, "               %01i", game->gamestate.lifes ? game->gamestate.lifes - 1 : 0);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 2.0f / WB_GRAPHIC_GUI_COLORMAP_SPEED, 
    -1.0f, offset_y,
    game->graphic.colormap.green8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
    
    // enemy_cnt
    sprintf(game->graphic.text, "                    %02i", game->gamestate.enemy_cnt);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 1.0f / WB_GRAPHIC_GUI_COLORMAP_SPEED, 
    -1.0f, offset_y,
    game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
    // HI

    // highscore

    // 2UP

    // score2

    // level
    uint32_t* colormap = (uint32_t*)((uint8_t*)game->graphic.colormap.red8 + game->gamestate.level % 3 * (sizeof game->graphic.colormap.red8));
    offset_y = 2.0f * WB_GRAPHIC_GUI_LEVEL_OFFSET_Y / window_height;
    sprintf(game->graphic.text, "       %01i", game->gamestate.level + 1);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 2.0f, 2.0f, 0,
    -1.0f, offset_y,
    colormap, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_LEVEL_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // cauldrons

}

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

    // Set the key color
    game->shader.loc.key_color = glGetUniformLocation(game->shader.program, "keyColor");
    glUniform4f(game->shader.loc.key_color, WB_GRAPHIC_KEY_COLOR_R, WB_GRAPHIC_KEY_COLOR_G, WB_GRAPHIC_KEY_COLOR_B, WB_GRAPHIC_KEY_COLOR_A);
    // Set the frame cnt
    game->shader.loc.time = glGetUniformLocation(game->shader.program, "time");
    glUniform1f(game->shader.loc.time, game->gamestate.time);
    // Set the key alpha
    game->shader.loc.key_alpha = glGetUniformLocation(game->shader.program, "keyAlpha");
    glUniform1f(game->shader.loc.key_alpha, -1.0);
    // Set dust texture size
    game->shader.loc.tex_size = glGetUniformLocation(game->shader.program, "texSize");
    glUniform1f(game->shader.loc.tex_size, WB_GRAPHIC_MAP_DUST_SPRITE_SIZE);
    // Set the key color mode
    game->shader.loc.key_color_mode = glGetUniformLocation(game->shader.program, "keyColorMode");
    glUniform1i(game->shader.loc.key_color_mode, WB_COLORMODE_CYCLE);
    // Set the window scale
    game->shader.loc.window_scale = glGetUniformLocation(game->shader.program, "windowScale");
    glUniform1f(game->shader.loc.window_scale, (float)WB_GRAPHIC_WINDOW_HEIGHT / game->window.height);
    // Set the replace color reflect height
    game->shader.loc.replace_color_mirror_height = glGetUniformLocation(game->shader.program, "replaceColorMirrorHeight");
    glUniform1f(game->shader.loc.replace_color_mirror_height, WB_GRAPHIC_WINDOW_HEIGHT - WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y + 1);
    // Set the replace color height
    game->shader.loc.replace_colorband_height = glGetUniformLocation(game->shader.program, "replaceColorbandHeight");
    glUniform1f(game->shader.loc.replace_colorband_height, WB_GRAPHIC_TEXT_COLORBAND_HEIGHT);
    // Set the replace color speed later
    game->shader.loc.replace_color_speed = glGetUniformLocation(game->shader.program, "replaceColorSpeed");
    // Set the replace color cnt later
    game->shader.loc.replace_color_cnt = glGetUniformLocation(game->shader.program, "replaceColorCnt");
    // Set the replace colors later
    game->shader.loc.replace_colors = glGetUniformLocation(game->shader.program, "replaceColors");
    // Set the subpixel cnt
    game->shader.loc.subpixel_cnt = glGetUniformLocation(game->shader.program, "subpixelCnt");
    glUniform1f(game->shader.loc.subpixel_cnt, WB_GRAPHIC_SUBPIXEL_CNT);

    // entity sprite height is centered in atlas. They get shifted up by half a pixel with + 2.0 in offset_y
    switch (game->gamestate.state) {
        case WB_GAMESTATE_TITLESCREEN:
        wbGameDrawText(game, "WIZBALL", WB_TEXT_TITLE, 1.0f, 1.0f, 0.0,
            0.0f, 2.0f - (2.0f * WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y + WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT) / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.all32, WB_GRAPHIC_COLORMAP_ALL32_CNT, WB_GRAPHIC_TEXT_MIRROR_COLORMAP_SPEED, WB_COLORMODE_SCROLL);
        wbGameDrawText(game, "top scores", WB_TEXT_SMALL, 1.0f, 1.0f, WB_GRAPHIC_TEXT_TOPSCORES_DRAW_TIME,
            0.0f, 2.0f - (2.0f * WB_GRAPHIC_TEXT_TOPSCORES_OFFSET_Y) / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue4, WB_GRAPHIC_COLORMAP_RGB4_CNT, WB_GRAPHIC_TEXT_PULSE_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        glUniform1f(game->shader.loc.replace_color_mirror_height, WB_GRAPHIC_WINDOW_HEIGHT - (WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y - WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE - 1));
        glUniform1f(game->shader.loc.time, game->gamestate.time - WB_GRAPHIC_TEXT_HIGHSCORE1_DRAW_TIME);
        wbGameDrawText(game, "1. 050000          ", WB_TEXT_DIGIT, 2.0f, 1.0f, WB_GRAPHIC_TEXT_HIGHSCORE1_DRAW_TIME,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.green4, WB_GRAPHIC_COLORMAP_RGB4_CNT, WB_GRAPHIC_TEXT_SCROLL_COLORMAP_SPEED, WB_COLORMODE_SCROLL);
        glUniform1f(game->shader.loc.time, game->gamestate.time);
        wbGameDrawText(game, "         SENSI SOFT", WB_TEXT_LARGE, 1.0f, 1.0f, WB_GRAPHIC_TEXT_HIGHSCORE2_DRAW_TIME,
            0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y + (WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE - WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / 2 + 1) / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        for (int i = 0; i < 6; i++) {
            glUniform1f(game->shader.loc.replace_color_mirror_height, WB_GRAPHIC_WINDOW_HEIGHT - (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i));
            double time = WB_GRAPHIC_TEXT_SCOREBOARD1_DRAW_TIME + i * WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_TIME;
            glUniform1f(game->shader.loc.time, game->gamestate.time - time);
            sprintf(game->graphic.text, "   %06i           %01i.               ", 20000, i + 2);
            wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, time,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_SCROLL_COLORMAP_SPEED, WB_COLORMODE_SCROLL);
        }
        for (int i = 0; i < 6; i++) {
            glUniform1f(game->shader.loc.replace_color_mirror_height, WB_GRAPHIC_WINDOW_HEIGHT - (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i - WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE - 5));
            double time = WB_GRAPHIC_TEXT_SCOREBOARD2_DRAW_TIME + (6 - 1 - i) * WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_TIME;
            glUniform1f(game->shader.loc.time, game->gamestate.time - time);
            sprintf(game->graphic.text, "%01i.                     %06i        ", i + 2, 20000);
            wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, time,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_SCROLL_COLORMAP_SPEED, WB_COLORMODE_SCROLL);
        }
        glUniform1f(game->shader.loc.time, game->gamestate.time);
        for (int i = 0; i < 6; i++) {
            i == 0 ? sprintf(game->graphic.text, "          %s                 %s %s", "jop", "krx", "nif") : 
                     sprintf(game->graphic.text, "          %s                 %s %s", "---", "---", "---");
            wbGameDrawText(game, game->graphic.text, WB_TEXT_SMALL, 1.0f, 1.0f, WB_GRAPHIC_TEXT_SCOREBOARD3_DRAW_TIME,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        }
        wbGameDrawText(game, "one  plr              two  plrs  ", WB_TEXT_SMALL, 1.0f, 1.0f, WB_GRAPHIC_TEXT_SCOREBOARD4_DRAW_TIME,
            0.0f, 2.0f - 2.0f * 255 / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.red4, WB_GRAPHIC_COLORMAP_RGB4_CNT, WB_GRAPHIC_TEXT_PULSE_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        break;
        
        case WB_GAMESTATE_GETREADY:
        wbGameDrawEntities(game);
        wbGameDrawText(game, "GET READY!", WB_TEXT_LARGE, 1.0f, 1.0f, 0,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        break;

        case WB_GAMESTATE_SPAWN:
        wbGameDrawDust(game);
        wbGameDrawMap(game, &game->graphic.background_atlas);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWizSpawn(game);
        wbGameDrawMap(game, &game->graphic.foreground_atlas);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_PLAY:
        wbGameDrawDust(game);
        wbGameDrawMap(game, &game->graphic.background_atlas);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWiz(game);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_DEATH:
        wbGameDrawDust(game);
        wbGameDrawMap(game, &game->graphic.background_atlas);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_GAMEOVER:
        wbGameDrawDust(game);
        wbGameDrawMap(game, &game->graphic.background_atlas);
        wbGameDrawGui(game);
        wbGameDrawText(game, "GAME OVER", WB_TEXT_LARGE, 1.0f, 1.0f, 0,
            0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_GAMEOVER_OFFSET_Y) / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        wbGameDrawText(game, "player one", WB_TEXT_SMALL, 1.0f, 1.0f, 0,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        break;
    }

    // TODO: display fps
    sprintf(game->graphic.text, "%04i    ", (int)(1.0 / game->gamestate.delta_time));
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0, 1.0, 0,
        1.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT,
        game->graphic.colormap.green8, 6, 12, WB_COLORMODE_CYCLE);

    // Swap front and back buffers
    // The front buffer is what's currently displayed, the back buffer is what's being rendered to
    // Swapping them prevents flickering
    glfwSwapBuffers(game->window.handle);
}

int wbGameRun() {
    WBGame game;
    if(!wbGameInit(&game)) {
        fprintf(stderr, "Failed to initialize game\n");
        wbGameUninit(&game);
        return -1;
    }

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(game.window.handle)) {
        static double prev_time = 0.0;
        double time = glfwGetTime();
        game.gamestate.delta_time = fmin(time - prev_time, 1.0 / WB_FPS_MIN);
        uint64_t time_frame = (uint64_t)round(glfwGetTime() * WB_FPS_MAX);
        static uint64_t last_time_frame;
        // Poll for and process events
        // This checks for input events (keyboard, mouse) and window events (resize, close)
        glfwPollEvents();

        // Run with fps
        if (time_frame == last_time_frame) continue;
        if (glfwGetKey(game.window.handle, GLFW_KEY_P)) game.gamestate.delta_time = 0.0001; /*TODO: for debug*/
        if (glfwGetKey(game.window.handle, GLFW_KEY_LEFT_BRACKET)) game.gamestate.delta_time = 0.001; /*TODO: for debug*/

        wbWindowLockAspectRatio(&game.window);

        // --- Fullscreen toggle on Alt+Enter or F11 ---
        static int prev_f11 = 0, prev_alt_enter = 0;
        int key_f11 = glfwGetKey(game.window.handle, GLFW_KEY_F11);
        int key_alt = glfwGetKey(game.window.handle, GLFW_KEY_LEFT_ALT) || glfwGetKey(game.window.handle, GLFW_KEY_RIGHT_ALT);
        int key_enter = glfwGetKey(game.window.handle, GLFW_KEY_ENTER);
        if ((key_f11 && !prev_f11) || ((key_alt && key_enter) && !(prev_alt_enter))) {
            wbWindowToggleFullscreen(&game.window);
            wbWindowLockAspectRatio(&game.window);
        }
        prev_f11 = key_f11;
        prev_alt_enter = key_alt && key_enter;
        // --- End fullscreen toggle ---

        // --- VSync toggle on Alt+V ---
        static int prev_alt_v = 0;
        int key_v = glfwGetKey(game.window.handle, GLFW_KEY_V);
        if ((key_alt && key_v) && !(prev_alt_v)) {
            game.window.vsync ^= 1;
            glfwSwapInterval(game.window.vsync);
        }
        prev_alt_v = key_alt && key_v;
        // --- End VSync toggle ---

        bool space_pressed = glfwGetKey(game.window.handle, GLFW_KEY_SPACE);

        switch (game.gamestate.state) {
            case WB_GAMESTATE_TITLESCREEN: // TODO: also switch to other titlescreens (player select, wiztips, ...)
            wbGameRender(&game);
            if (space_pressed && !game.window.prev_key_state[GLFW_KEY_SPACE]) {
                wbGamestateSetupGetready(&game.gamestate, &game.sound, &game.map.view, &game.enemy_buffer, &game.particle_buffer, &game.projectile_buffer);
            }
            game.window.prev_key_state[GLFW_KEY_SPACE] = space_pressed;
            break;

            case WB_GAMESTATE_GETREADY:
            uint32_t seed = glfwGetTime() * 1e9;
            if (randfin(seed, 0.0f, 1.0f) < 0.0625f * game.gamestate.delta_time * 50) {
                wbEnemyInsertRandoms(&game.enemy_buffer, game.gamestate.time);
            }
            wbGameRender(&game);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            if (space_pressed && !game.window.prev_key_state[GLFW_KEY_SPACE]) {
                ma_sound_stop(&game.sound.getready);
                game.gamestate.time = 0.0;
                game.gamestate.powerup.slot = -1;
                game.gamestate.powerup.unlocked = game.gamestate.powerup.permanent;
                int pos_x_min = WB_GRAPHIC_MAP_VIEW_WIDTH / 2 - 1;
                int pos_x_max = game.graphic.background_atlas.width - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + 1;
                wbPlayerWizInit(&game.player.wiz, pos_x_min, pos_x_max);
                wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
                wbPlayerCatInit(&game.player.cat);
                wbBufferClear(&game.enemy_buffer);
                wbEnemyPopulate(&game.enemy_buffer, WB_ENEMY_SPINNERCYAN, WB_GRAPHIC_ENEMY_COLORMAP_CYAN_OFFSET, WB_MOVEPATTERN_INERT, &game.map.view);
                game.gamestate.state = WB_GAMESTATE_SPAWN;
            }
            game.window.prev_key_state[GLFW_KEY_SPACE] = space_pressed;
            break;

            case WB_GAMESTATE_SPAWN:
            wbGameProcessInput(&game);
            wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate);
            wbParticleUpdate(&game.particle_buffer, &game.player, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            wbGameRender(&game);
            if (game.gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED >= WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_FRAME_CNT) {
                game.player.wiz.animation_angle = 0.0f;
                game.gamestate.state = WB_GAMESTATE_PLAY;
            }
            break;

            case WB_GAMESTATE_PLAY:
            if (game.gamestate.enemy_cnt == 0) { // TODO: and no powerup in gamestate
                ma_sound_seek_to_pcm_frame(&game.sound.clear, 0);
                ma_sound_start(&game.sound.clear);
                wbEnemyPopulate(&game.enemy_buffer, WB_ENEMY_CIRCLE, WB_GRAPHIC_ENEMY_COLORMAP_OFFSET, WB_MOVEPATTERN_CIRCLE, &game.map.view);
                wbEnemyPopulate(&game.enemy_buffer, WB_ENEMY_DROPLET, WB_GRAPHIC_ENEMY_COLORMAP_RED_OFFSET, WB_MOVEPATTERN_CIRCLE, &game.map.view);
            }
            wbGameProcessInput(&game);
            wbPlayerWizHandleCollision(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate);
            wbParticleUpdate(&game.particle_buffer, &game.player, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            wbGameRender(&game);
            if (!game.player.cat.health) {
                game.gamestate.powerup.unlocked &= ~WB_POWERUP_CAT;
                // TODO: play catdeath sound
                wbPlayerCatInit(&game.player.cat);
            }
            if (!game.player.wiz.health) {
                ma_sound_stop(&game.sound.fire);
                ma_sound_stop(&game.sound.powerup_activate);
                ma_sound_stop(&game.sound.powerup_collect);
                ma_sound_stop(&game.sound.powerup_drop);
                ma_sound_stop(&game.sound.decay);
                ma_sound_stop(&game.sound.cathit);
                ma_sound_seek_to_pcm_frame(&game.sound.wizdeath, 0);
                ma_sound_start(&game.sound.wizdeath);
                game.player.wiz.vel.x = 0.0f;
                game.gamestate.lifes--;
                game.gamestate.time = 0.0;
                game.gamestate.state = WB_GAMESTATE_DEATH;
            }
            break;

            case WB_GAMESTATE_DEATH:
            wbGameProcessInput(&game);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate);
            wbParticleUpdate(&game.particle_buffer, &game.player, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            game.gamestate.lifes++;
            wbGameRender(&game);
            game.gamestate.lifes--;
            if (game.gamestate.time >= WB_GAMERULE_GAMESTATE_HIT_TIME) {
                if (game.gamestate.lifes) {
                    wbGamestateSetupGetready(&game.gamestate, &game.sound, &game.map.view, &game.enemy_buffer, &game.particle_buffer, &game.projectile_buffer);
                } else {
                    ma_sound_seek_to_pcm_frame(&game.sound.gameover, 0);
                    ma_sound_start(&game.sound.gameover);
                    game.gamestate.time = 0.0;
                    game.gamestate.state = WB_GAMESTATE_GAMEOVER;
                }
            }
            break;

            case WB_GAMESTATE_GAMEOVER:
            wbGameRender(&game);
            if (game.gamestate.time >= WB_GAMERULE_GAMESTATE_GAMEOVER_TIME) {
                wbGamestateSetupTitlescreen(&game.gamestate, &game.sound);
            }
            break;
        }

        last_time_frame = time_frame;
        prev_time = time;
        game.gamestate.time += game.gamestate.delta_time;
    }

    wbGameUninit(&game);
    return 0;
}
