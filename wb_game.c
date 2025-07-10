#include "wizball.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef WB_NO_SOUND
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#endif

// TODO: make down press bounce height from terrain consistent using +.5 acc, use vel, +.5 acc

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
    game->gamestate.delta_time = 0.0;
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
    static const double process_input_time = 1.0 / WB_GAMERULE_PROCESS_INPUT_SPEED;
    static double timer = process_input_time;
    timer += game->gamestate.delta_time;
    if (timer < process_input_time) return;
    timer -= process_input_time;

    static bool wiz_fire_prev = false;
    static bool cat_fire_prev = false;

    static bool key_powerup_togglegrav_prev = false;
    static bool key_powerup_left_prev       = false;
    static bool key_powerup_right_prev      = false;
    static bool key_powerup_prev            = false;
    static bool key_wiz_left_prev           = false;
    static bool key_wiz_right_prev          = false;
    static bool key_wiz_fire_prev           = false;
    static bool key_cat_fire_prev           = false;

    bool key_alt =                glfwGetKey(game->window.handle, WB_KEY_ALTL     )          || glfwGetKey(game->window.handle, WB_KEY_ALTR);
    bool key_wiz_up =             glfwGetKey(game->window.handle, WB_KEY_WIZ_UP   )          && !key_alt;
    bool key_wiz_down =           glfwGetKey(game->window.handle, WB_KEY_WIZ_DOWN )          && !key_alt;
    bool key_wiz_left =           glfwGetKey(game->window.handle, WB_KEY_WIZ_LEFT )          && !key_alt;
    bool key_wiz_right =          glfwGetKey(game->window.handle, WB_KEY_WIZ_RIGHT)          && !key_alt;
    bool key_wiz_fire =           glfwGetKey(game->window.handle, WB_KEY_WIZ_FIRE )          && !key_alt;
    bool key_cat_up =             glfwGetKey(game->window.handle, WB_KEY_CAT_UP   )          && !key_alt;
    bool key_cat_down =           glfwGetKey(game->window.handle, WB_KEY_CAT_DOWN )          && !key_alt;
    bool key_cat_left =           glfwGetKey(game->window.handle, WB_KEY_CAT_LEFT )          && !key_alt;
    bool key_cat_right =          glfwGetKey(game->window.handle, WB_KEY_CAT_RIGHT)          && !key_alt;
    bool key_cat_fire =           glfwGetKey(game->window.handle, WB_KEY_CAT_FIRE)           && !key_alt;
         key_cat_fire |=          glfwGetMouseButton(game->window.handle, WB_MOUSE_CAT_FIRE) && !key_alt;
    bool key_wiz_sprint =         glfwGetKey(game->window.handle, WB_KEY_SPRINT  )           && !key_alt; // TODO: not in real game
    bool key_powerup_togglegrav = glfwGetKey(game->window.handle, WB_KEY_POWERUP_TOGGLEGRAV) && !key_alt; // TODO: not in real game
    bool key_powerup_left =       glfwGetKey(game->window.handle, WB_KEY_POWERUP_LEFT)       && !key_alt; // TODO: not in real game
    bool key_powerup_right =      glfwGetKey(game->window.handle, WB_KEY_POWERUP_RIGHT)      && !key_alt; // TODO: not in real game
    bool key_powerup =            glfwGetKey(game->window.handle, WB_KEY_POWERUP      )      && !key_alt; // TODO: not in real game

    WBWiz* wiz   = &game->player.wiz;
    WBCat* cat   = &game->player.cat;
    WBView* view = &game->map.view;

    // wbGamestatePowerupProcessInput
    static int    wiggle_cnt  = 0;
    static double wiggle_time = 0;
    static WBDirectionType wiggle_dir_prev = WB_DIRECTION_NEUTRAL;

    WBPowerup* powerup = &game->gamestate.powerup;

    static double left_down_time;
    if (key_wiz_left && !key_wiz_left_prev) {
        left_down_time = game->gamestate.time;
    }
    left_down_time *= key_wiz_left;
    key_wiz_left_prev = key_wiz_left;

    static double right_down_time;
    if (key_wiz_right && !key_wiz_right_prev) {
        right_down_time = game->gamestate.time;
    }
    right_down_time *= key_wiz_right;
    key_wiz_right_prev = key_wiz_right;

    if (!wiggle_cnt) {
        wiggle_time = game->gamestate.time;
    }
    WBDirectionType wiggle_dir = left_down_time > 0   || right_down_time > 0 ?
                                 left_down_time        > right_down_time     ?
                                 WB_DIRECTION_NEGATIVE : WB_DIRECTION_POSITIVE : WB_DIRECTION_NEUTRAL;
    if (wiggle_dir != WB_DIRECTION_NEUTRAL && wiggle_dir != wiggle_dir_prev) {
        if (game->gamestate.time - wiggle_time < 1.0f / WB_GAMERULE_POWERUP_WIGGLE_SPEED) {
            wiggle_dir_prev = wiggle_dir;
            wiggle_time = game->gamestate.time;
            wiggle_cnt++;
        }
        else {
            wiggle_dir_prev = WB_DIRECTION_NEUTRAL;
            wiggle_cnt = 0;
        }
    }

    key_powerup &= !key_powerup_prev;
    key_powerup_prev = key_powerup;
    if (wiggle_cnt >= WB_GAMERULE_POWERUP_WIGGLE_CNT) {
        key_powerup = true;
        wiggle_cnt = 0;
    }
    int powerup_slotstate = (powerup->unlocked >> 2 * powerup->slot) & WB_POWERUP_SLOTMASK;
    if (key_powerup && game->gamestate.state == WB_GAMESTATE_PLAY && powerup->slot >= 0 &&
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

    // TODO: for debug: toggle powerup on strg
    powerup->slot -= key_powerup_left  && !key_powerup_left_prev;
    powerup->slot += key_powerup_right && !key_powerup_right_prev;
    key_powerup_left_prev  = key_powerup_left;
    key_powerup_right_prev = key_powerup_right;
    if (key_powerup_togglegrav && !key_powerup_togglegrav_prev)
        powerup->unlocked ^= WB_POWERUP_SLOTMASK;
    key_powerup_togglegrav_prev = key_powerup_togglegrav;

    // wbPlayerWizProcessInput
    static double wiz_fire_down_time = 1e18;
    if (key_wiz_fire && !key_wiz_fire_prev) {
        wiz_fire_down_time = game->gamestate.time;
    }
    bool autofire = key_wiz_fire
                 && game->gamestate.time - wiz_fire_down_time >= WB_GAMERULE_AUTOFIRE_HOLD_TIME
                 && powerup->unlocked & WB_POWERUP_CAT;
    if (autofire) {
        key_cat_right |= key_wiz_right;
        key_cat_left  |= key_wiz_left;
        key_cat_up    |= key_wiz_up;
        key_cat_down  |= key_wiz_down;
        key_wiz_right  = false;
        key_wiz_left   = false;
        key_wiz_up     = false;
        key_wiz_down   = false;
    }
    if (key_wiz_right) {
        wiz->vel_x_key += WB_GAMERULE_PLAYER_WIZ_ACC_X * process_input_time;
        wiz->vel_x_key = fminf(wiz->vel_x_key,   WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !key_wiz_sprint);
    }
    if (key_wiz_left) {
        wiz->vel_x_key -= WB_GAMERULE_PLAYER_WIZ_ACC_X * process_input_time;
        wiz->vel_x_key = fmaxf(wiz->vel_x_key, -(WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !key_wiz_sprint));
    }

    if (powerup->unlocked & WB_POWERUP_ANTIGRAV) {
        if (key_wiz_down) {
            wiz->vel_y_key += WB_GAMERULE_PLAYER_WIZ_ACC_Y * process_input_time;
            wiz->vel_y_key = fminf(wiz->vel_y_key,  WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT - 1);
        }
        if (key_wiz_up) {
            wiz->vel_y_key -= WB_GAMERULE_PLAYER_WIZ_ACC_Y * process_input_time;
            wiz->vel_y_key = fmaxf(wiz->vel_y_key, -WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT + 1);
        }
        float vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
        wiz->vel_x_key -= fsgnf(vel_x) * WB_GAMERULE_PLAYER_WIZ_DEC_X * process_input_time * (!key_wiz_left && !key_wiz_right);
        float vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
        wiz->vel_y_key -= fsgnf(vel_y) * WB_GAMERULE_PLAYER_WIZ_DEC_Y * process_input_time * (!key_wiz_up && !key_wiz_down);
    }

    bool mute_wiz_fire = wiz_fire_prev && !cat_fire_prev;
    wiz_fire_prev = false;
    if ((key_wiz_fire && !key_wiz_fire_prev || autofire) && game->gamestate.time &&
        (game->gamestate.state == WB_GAMESTATE_PLAY || game->gamestate.state == WB_GAMESTATE_SPAWN)) {
        
        WBVec2f vel;
        if (view->bullet_wiz_cnt < WB_GAMERULE_VIEW_BULLET_WIZ_CNT_MAX) {
            if (!mute_wiz_fire) {
                ma_sound_seek_to_pcm_frame(&game->sound.fire, 0);
                ma_sound_start(&game->sound.fire);
            }
            vel.x = powerup->unlocked & WB_POWERUP_DOUBLE ?
                WB_GAMERULE_PROJECTILE_VEL * wiz->next_bullet_direction : WB_GAMERULE_PROJECTILE_VEL * wiz->facing;
            vel.y = 0.0f;
            WBProjectileType type = powerup->unlocked & WB_POWERUP_BLAZERS ? WB_PROJECTILE_BLAZER_WIZ : WB_PROJECTILE_BULLET_WIZ;
            wbProjectileAppend(&game->projectile_buffer, type, &wiz->pos, &vel);
            wiz->next_bullet_direction *= -1;
            wiz_fire_prev = true;
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

    // wbPlayerCatProcessInput
    if (!(powerup->unlocked & WB_POWERUP_CAT)) {
        key_wiz_fire_prev = key_wiz_fire;
        return;
    }
    
    if (glfwGetMouseButton(game->window.handle, WB_MOUSE_CAT_MOVE)) {
        double mouse_pos_x, mouse_pos_y;
        glfwGetCursorPos(game->window.handle, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x -= game->window.viewport.pos_x;
        mouse_pos_y -= game->window.viewport.pos_y;
        mouse_pos_x *= (double)WB_GRAPHIC_WINDOW_WIDTH  / game->window.viewport.width;
        mouse_pos_y *= (double)WB_GRAPHIC_WINDOW_HEIGHT / game->window.viewport.height;
        mouse_pos_x -= 0.5f * WB_GRAPHIC_WINDOW_WIDTH - view->center_x;
        mouse_pos_y -= WB_GRAPHIC_WINDOW_HEIGHT - WB_GRAPHIC_MAP_VIEW_HEIGHT - WB_GRAPHIC_MAP_VIEW_OFFSET_Y;
        WBVec2f vel = {
            (mouse_pos_x - cat->pos.x),
            (mouse_pos_y - cat->pos.y)
        };
        float rnorm = rsqrtf(vel.x * vel.x + vel.y * vel.y);
        if (rnorm > 1.5f / WB_GRAPHIC_SUBPIXEL_CNT) {
            vel.x = 0;
            vel.y = 0;
        } else if (rnorm < 1.0 / (WB_GAMERULE_PLAYER_CAT_VEL * process_input_time)) {
            vel.x *= rnorm * WB_GAMERULE_PLAYER_CAT_VEL;
            vel.y *= rnorm * WB_GAMERULE_PLAYER_CAT_VEL;
        } else {
            vel.x *= WB_GAMERULE_PROCESS_INPUT_SPEED;
            vel.y *= WB_GAMERULE_PROCESS_INPUT_SPEED;
        }
        cat->vel.x = vel.x;
        cat->vel.y = vel.y;
        cat->retreat = false;
    } else {
        cat->vel.x = 0.0f;
        cat->vel.y = 0.0f;
        cat->vel.x += WB_GAMERULE_PLAYER_CAT_VEL * key_cat_right;
        cat->vel.x -= WB_GAMERULE_PLAYER_CAT_VEL * key_cat_left;
        cat->vel.y += WB_GAMERULE_PLAYER_CAT_VEL * key_cat_down;
        cat->vel.y -= WB_GAMERULE_PLAYER_CAT_VEL * key_cat_up;
        cat->retreat = !(key_cat_right || key_cat_left || key_cat_up || key_cat_down);
    }

    bool mute_cat_fire = cat_fire_prev && !wiz_fire_prev;
    cat_fire_prev = false;
    if (key_cat_fire && !key_cat_fire_prev ||
        key_wiz_fire && !key_wiz_fire_prev || autofire) {
        WBVec2f vel;
        if (view->bullet_cat_cnt < WB_GAMERULE_VIEW_BULLET_CAT_CNT_MAX) {
            if (!mute_cat_fire) {
                ma_sound_seek_to_pcm_frame(&game->sound.fire, 0);
                ma_sound_start(&game->sound.fire);
            }
            vel.x = WB_GAMERULE_PROJECTILE_VEL * cat->facing;
            vel.y = 0.0f;
            WBProjectileType type = powerup->unlocked & WB_POWERUP_BLAZERS ? WB_PROJECTILE_BLAZER_CAT : WB_PROJECTILE_BULLET_CAT;
            wbProjectileAppend(&game->projectile_buffer, type, &cat->pos, &vel);
            cat_fire_prev = true;
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

    key_wiz_fire_prev = key_wiz_fire;
    key_cat_fire_prev = key_cat_fire;
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
    
    static float replace_colors[WB_GRAPHIC_COLORMAP_ALL32_CNT * WB_GRAPHIC_RGBA_CNT];
    if (color_mode == 1) {
        int cnt = (game->gamestate.time - draw_time) * color_speed / WB_GRAPHIC_TEXT_COLORBAND_HEIGHT + 1;
        cnt = cnt > color_cnt ? color_cnt : cnt;
        for (int i = 0; i < color_cnt; i++) {
            uint32_t color = i >= cnt ? 0x000000FF : colors[i];
            int idx = (-i + color_cnt) % color_cnt;
            ui32toarr4f(&replace_colors[idx * WB_GRAPHIC_RGBA_CNT], color);
        }
        glUniform1f(game->shader.loc.replace_color_speed, color_speed);
        glUniform1i(game->shader.loc.replace_color_cnt, color_cnt);
    } else {
        int idx = (uint64_t)((game->gamestate.time - draw_time) * color_speed) % color_cnt;
        ui32toarr4f(replace_colors, colors[idx]);
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
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH * width_scale;
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
                offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_WIDTH, offset_u,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT, WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH * width_scale;
        }
        break;

        case WB_TEXT_SMALL:
        string_width = strlen(text);
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH;
        for (char* p = text; *p; p++) {
            if (*p != ' ') {
                i = *p == '-' ? WB_GRAPHIC_TEXT_SMALL_MINUS_SPRITE_ATLAS_OFFSET : *p - 'a';
                offset_u = (WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE) / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_WIDTH, offset_u,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT, WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH;
        }
        break;

        case WB_TEXT_LARGE:
        string_width = 0.0f;
        char char_ws[WB_GRAPHIC_TEXT_CHAR_CNT];
        for (int idx = 0; text[idx]; idx++) {
            char_ws[idx] = text[idx] == 'I' || text[idx] == 'L'? 0.5f : 1.0f;
            string_width += char_ws[idx];
        }
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH;
        for (int idx = 0; text[idx]; idx++) {
            if (text[idx] != ' ') {
                i = text[idx] == '!' ? 26 : text[idx] - 'A';
                offset_u = (WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE) / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH  * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT * height_scale, offset_y,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_WIDTH, offset_u,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT, WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
            }
            _offset_x += 2.0f * char_ws[idx] * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH;
        }
        break;

        case WB_TEXT_TITLE:
        wbGameDrawBatchAppend(&game->shader,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH  / WB_GRAPHIC_WINDOW_WIDTH  * width_scale,  offset_x,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / WB_GRAPHIC_WINDOW_HEIGHT * height_scale, offset_y,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH  / WB_GRAPHIC_SPRITE_ATLAS_WIDTH,  WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT, WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
        break;
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);
}
        
void wbGameDrawDust(WBGame* game) {
    WBGraphic* graphic = &game->graphic;
    WBView* view = &game->map.view;
    static const float width_x = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X / WB_GRAPHIC_WINDOW_WIDTH;
    static const float height_y = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y / WB_GRAPHIC_WINDOW_HEIGHT;
    static const float width_u = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE / WB_GRAPHIC_MAP_DUST_ATLAS_WIDTH;
    float offset_x;
    float offset_y;
    float offset_u;
    wbGameDrawBatchClear(&game->shader);
    for (int layer = 0; layer < WB_GRAPHIC_MAP_DUST_LAYER_CNT; layer++) {
        for (int row = 0; row < WB_GRAPHIC_MAP_DUST_ROW_CNT; row++) {
            for (int col = 0; col < WB_GRAPHIC_MAP_DUST_COL_CNT; col++) {
                offset_x = (-2.0f * roundf(view->center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT
                            -2.0f * roundf(WB_GRAPHIC_MAP_DUST_VELOCITY_FACTOR * layer * view->center_x / WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT)
                           / WB_GRAPHIC_WINDOW_WIDTH + width_x + 2.0f * col * width_x;
                offset_x = fmodf(offset_x, 2.0f * WB_GRAPHIC_MAP_DUST_COL_CNT * width_x);
                offset_x += (row / (WB_GRAPHIC_MAP_DUST_ROW_CNT - 1.0f) - 0.5f) * width_x;
                offset_x += (offset_x < -WB_GRAPHIC_MAP_DUST_COL_CNT * width_x) ? 2.0f * WB_GRAPHIC_MAP_DUST_COL_CNT * width_x : 0.0f;
                offset_y = 2.0f * (WB_GRAPHIC_MAP_VIEW_OFFSET_Y + WB_GRAPHIC_MAP_VIEW_HEIGHT) / WB_GRAPHIC_WINDOW_HEIGHT - 2.0f * height_y - 2.0f * row * height_y;
                offset_u = layer * width_u;
                wbGameDrawBatchAppend(&game->shader,
                    width_x, offset_x, height_y, offset_y, width_u, offset_u, WB_GRAPHIC_MAP_DUST_SPRITE_SIZE / WB_GRAPHIC_MAP_DUST_ATLAS_HEIGHT, 0.0f);
            }
        }
        if (layer == 0) {
            wbGameDrawBatch(&game->shader, graphic->dust_atlas_texture_id);
            wbGameDrawBatchClear(&game->shader);
            glUniform1f(game->shader.loc.key_alpha, 128.0/255.0);
        }
    }
    wbGameDrawBatch(&game->shader, graphic->dust_atlas_texture_id);
    glUniform1f(game->shader.loc.key_alpha, -1.0);
}

void wbGameDrawMap(WBGame* game, GLuint texture_id) {
    WBView* view = &game->map.view;
    static const float width_u = WB_GRAPHIC_MAP_VIEW_WIDTH / WB_GRAPHIC_MAP_ATLAS_WIDTH;
    float offset_u = roundf(view->center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT / WB_GRAPHIC_MAP_ATLAS_WIDTH - 0.5f * width_u;
    wbGameDraw(&game->shader, texture_id,
        WB_GRAPHIC_MAP_VIEW_WIDTH / WB_GRAPHIC_WINDOW_WIDTH  , 0.0f,
        WB_GRAPHIC_MAP_VIEW_HEIGHT / WB_GRAPHIC_WINDOW_HEIGHT, 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
        width_u                                              , offset_u,
        1.0f / WB_MAP_CNT                                    , (float)game->gamestate.level / WB_MAP_CNT);
}

void wbGameDrawEntities(WBGame* game) {
    WBProjectile* projectile;
    WBParticle* particle;
    WBEnemy* enemy;

    float offset_x, offset_y, offset_u, offset_v;
    float rgba[WB_GRAPHIC_RGBA_CNT];
    uint32_t color;
    uint32_t color_prev;

    // particles
    wbGameDrawBatchClear(&game->shader);
    glGetUniformfv(game->shader.program, game->shader.loc.replace_colors, rgba);
    color_prev = arr4ftoui32(rgba);
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &game->particle_buffer.entries[i];
        switch ((WBParticleType)particle->head.type) {
            case WB_PARTICLE_NONE:
            continue;

            case WB_PARTICLE_POWERUP:
            offset_u = WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;

            case WB_PARTICLE_DECAY:
            offset_u = WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)particle->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
            offset_v = WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            break;

            case WB_PARTICLE_DROPLET_FALL:
            offset_u = WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)particle->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
            offset_v = WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            break;
            
            case WB_PARTICLE_DROPLET_SPLAT:
            offset_u = WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)particle->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
            offset_v = WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            break;
        }
        offset_x = 2.0f * roundf((particle->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
        offset_y =  WB_GRAPHIC_ENTITY_OFFSET - 2.0f * (roundf(particle->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
        color = game->graphic.colormap.enemy[(int)particle->head.color_key];
        if (color != color_prev) {
            color_prev = color;
            wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);
            wbGameDrawBatchClear(&game->shader);
            ui32toarr4f(rgba, color);
            glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
        }
        wbGameDrawBatchAppend(&game->shader,
            WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
            WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);

    // enemies
    wbGameDrawBatchClear(&game->shader);
    glGetUniformfv(game->shader.program, game->shader.loc.replace_colors, rgba);
    color_prev = arr4ftoui32(rgba);
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &game->enemy_buffer.entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERCYAN:
            offset_u = WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)enemy->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
            offset_v = WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;

            case WB_ENEMY_CIRCLE:
            offset_u = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;

            case WB_ENEMY_DROPLET:
            offset_u = WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)enemy->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
            offset_v = WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
        }
        offset_x = 2.0f * roundf((enemy->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
        offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(enemy->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
        color = game->graphic.colormap.enemy[(int)enemy->head.color_key];
        if (color != color_prev) {
            color_prev = color;
            wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);
            wbGameDrawBatchClear(&game->shader);
            ui32toarr4f(rgba, color);
            glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
        }
        wbGameDrawBatchAppend(&game->shader,
            WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
            WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);

    // projectiles
    wbGameDrawBatchClear(&game->shader);
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &game->projectile_buffer.entries[i];
        switch (projectile->head.type) {
            case WB_PROJECTILE_NONE: continue;
            case WB_PROJECTILE_BULLET_WIZ:
            offset_u = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_BULLET_CAT:
            offset_u = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_BLAZER_WIZ:
            offset_u = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_BLAZER_CAT:
            offset_u = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_SPRAY_NW:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_SPRAY_N:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_SPRAY_NE:
            offset_u = WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
            offset_v = WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
            case WB_PROJECTILE_BEAM:
                int color_key = (int)projectile->head.color_key % 2 ? projectile->head.color_key / 4 + 1 : 0;
                uint32_t color = game->graphic.colormap.beam[color_key];
                ui32toarr4f(rgba, color);
                glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
                projectile->head.pos.y -= 0.5f * WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
                offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + (int)projectile->head.animation_key * WB_GRAPHIC_SPRITE_WIDTH_U;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
                wbGameDrawBatchAppend(&game->shader,
                    WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
                    WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
                projectile->head.pos.y += WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + ((int)projectile->head.animation_key + WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT) * WB_GRAPHIC_SPRITE_WIDTH_U;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
            break;
        }
        offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
        offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
        wbGameDrawBatchAppend(&game->shader,
            WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
            WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);
}

void wbGameDrawPlayerCat(WBGame* game) {
    WBCat* cat = &game->player.cat;
    WBWiz* wiz = &game->player.wiz;
    float offset_x = 2.0f * roundf((cat->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
    float offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(cat->pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
    float offset_u = WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH
                   + (uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_CAT_ANIMATION_SPEED + 0.5) % WB_GRAPHIC_PLAYER_CAT_ANIMATION_FRAME_CNT * WB_GRAPHIC_SPRITE_WIDTH_U;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas_texture_id,
        WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
        WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
}

void wbGameDrawPlayerWiz(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;
    float offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
    float offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
    float offset_u = WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH + roundf(wiz->animation_angle) * WB_GRAPHIC_SPRITE_WIDTH_U;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas_texture_id,
        WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
        WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
}

void wbGameDrawPlayerWizSpawn(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;
    float offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_WIDTH;
    float offset_y = WB_GRAPHIC_ENTITY_OFFSET  - 2.0f * (roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT + 0.5f) - 0.5f) * WB_GRAPHIC_SUBPIXEL_CNT / WB_GRAPHIC_WINDOW_HEIGHT;
    float offset_u = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH
                   + ((uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) % (uint64_t)(WB_GRAPHIC_SPRITE_ATLAS_WIDTH  / WB_GRAPHIC_SPRITE_SIZE)) * WB_GRAPHIC_SPRITE_WIDTH_U;
    float offset_v = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT
                   + ((uint64_t)(game->gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) / (uint64_t)(WB_GRAPHIC_SPRITE_ATLAS_HEIGHT / WB_GRAPHIC_SPRITE_SIZE)) * WB_GRAPHIC_SPRITE_HEIGHT_V;
    wbGameDraw(&game->shader, game->graphic.sprite_atlas_texture_id,
        WB_GRAPHIC_SPRITE_WIDTH_X , offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
        WB_GRAPHIC_SPRITE_WIDTH_U , offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
}

void wbGameDrawBatchAppendPowerupSlot(WBGame* game, int slot) {
    WBGraphic* graphic = &game->graphic;
    float offset_x = -1.0f + (WB_GRAPHIC_WINDOW_WIDTH - (WB_POWERUP_SLOT_CNT - 1) * WB_GRAPHIC_GUI_POWERUP_STRIDE) / WB_GRAPHIC_WINDOW_WIDTH
                    + 2.0f * slot * WB_GRAPHIC_GUI_POWERUP_STRIDE / WB_GRAPHIC_WINDOW_WIDTH;
    float offset_y =  2.0f - 2.0f * WB_GRAPHIC_GUI_POWERUP_STRIDE / WB_GRAPHIC_WINDOW_HEIGHT;
    WBPowerupType powerup_slotstate = ((game->gamestate.powerup.unlocked >> 2 * slot) & WB_POWERUP_SLOTMASK);
    if (slot == (int)(0.5f * log2f(WB_POWERUP_WIZSPRAY + 1))) {
        powerup_slotstate &= 1;
    }
    float offset_u = powerup_slotstate < 2 ?
                     ((2.0f * slot + powerup_slotstate) * WB_GRAPHIC_SPRITE_SIZE + WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X) / WB_GRAPHIC_SPRITE_ATLAS_WIDTH
                   : WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH;
    static const float offset_v = WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT;
    wbGameDrawBatchAppend(&game->shader,
        WB_GRAPHIC_SPRITE_WIDTH_X, offset_x, WB_GRAPHIC_SPRITE_HEIGHT_Y, offset_y,
        WB_GRAPHIC_SPRITE_WIDTH_U, offset_u, WB_GRAPHIC_SPRITE_HEIGHT_V, offset_v);
}

void wbGameDrawGui(WBGame* game) {
    float rgba[WB_GRAPHIC_RGBA_CNT];

    // powerup inactive slots
    wbGameDrawBatchClear(&game->shader);
    int slot = 0;
    int powerup_slotstate;
    uint32_t color = game->graphic.colormap.blue6[1];
    ui32toarr4f(rgba, color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    for (; slot < WB_POWERUP_SLOT_CNT; slot++) {
        if (slot == game->gamestate.powerup.slot) continue;
        wbGameDrawBatchAppendPowerupSlot(game, slot);
    }
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);
    // powerup active slot
    wbGameDrawBatchClear(&game->shader);
    slot = game->gamestate.powerup.slot;
    color = game->graphic.colormap.blue6[(uint64_t)(game->gamestate.time * WB_GRAPHIC_GUI_COLORMAP_SPEED + 0.5) % WB_GRAPHIC_COLORMAP_BLUE6_CNT];
    ui32toarr4f(rgba, color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    wbGameDrawBatchAppendPowerupSlot(game, game->gamestate.powerup.slot);
    wbGameDrawBatch(&game->shader, game->graphic.sprite_atlas_texture_id);

    // 1UP
    static const float offset_y = 2.0f - 2.0f * WB_GRAPHIC_GUI_SCORE_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT;
    wbGameDrawText(game, "   1UP", WB_TEXT_DIGIT, 1.0f, 1.0f, 0,
        -1.0f, offset_y,
        game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // score
    static const float stride_y = 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT;
    sprintf(game->graphic.text, "      %06i", game->gamestate.score);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 0,
        -1.0f, offset_y - stride_y,
        game->graphic.colormap.red8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // lifes
    sprintf(game->graphic.text, "               %01i", game->gamestate.lifes ? game->gamestate.lifes - 1 : 0);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 2.0f / WB_GRAPHIC_GUI_COLORMAP_SPEED, 
    -1.0f, offset_y - stride_y,
    game->graphic.colormap.green8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
    
    // enemy_cnt
    sprintf(game->graphic.text, "                    %02i", game->gamestate.enemy_cnt);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0f, 1.0f, 1.0f / WB_GRAPHIC_GUI_COLORMAP_SPEED, 
    -1.0f, offset_y - stride_y,
    game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
    // HI

    // highscore

    // 2UP

    // score2

    // level
    uint32_t* colormap = (uint32_t*)((uint8_t*)game->graphic.colormap.red8 + game->gamestate.level % 3 * (sizeof game->graphic.colormap.red8));
    sprintf(game->graphic.text, "%01i", game->gamestate.level + 1);
    wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 3.0f, 2.0f, 0,
        -1.0f + 2.0f * WB_GRAPHIC_GUI_LOWER_OFFSET_X / WB_GRAPHIC_WINDOW_WIDTH,
                2.0f * WB_GRAPHIC_GUI_LEVEL_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
        colormap, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_GUI_LEVEL_COLORMAP_SPEED, WB_COLORMODE_CYCLE);

    // cauldrons
    glUniform1i(game->shader.loc.key_color_mode, WB_COLORMODE_FILL);
    float offset_x = -2.0f * WB_GRAPHIC_GUI_CAULDRON_STRIDE / WB_GRAPHIC_WINDOW_WIDTH;
    for (int i = 0; i < 3; i++) {
        color = game->graphic.colormap.enemy[WB_GRAPHIC_ENEMY_COLORMAP_RED_OFFSET + i];
        ui32toarr4f(rgba, color);
        glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
        glUniform1f(game->shader.loc.fill_level, game->gamestate.cauldron_levels[i]);
    wbGameDraw(&game->shader, game->graphic.sprite_atlas_texture_id,
        WB_GRAPHIC_SPRITE_WIDTH_X , offset_x,
        WB_GRAPHIC_SPRITE_HEIGHT_Y, 0.0,
        WB_GRAPHIC_SPRITE_WIDTH_U , WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH,
        WB_GRAPHIC_SPRITE_HEIGHT_V, WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
        offset_x += 2.0f * WB_GRAPHIC_GUI_CAULDRON_STRIDE / WB_GRAPHIC_WINDOW_WIDTH;
    }
    offset_x = 1.0f - 2.0f * WB_GRAPHIC_GUI_LOWER_OFFSET_X / WB_GRAPHIC_WINDOW_WIDTH;
    color = game->graphic.colormap.enemy[WB_GRAPHIC_ENEMY_COLORMAP_RED_OFFSET];
    ui32toarr4f(rgba, color);
    glUniform4fv(game->shader.loc.replace_colors, 1, rgba);
    glUniform1f(game->shader.loc.fill_level, game->gamestate.cauldron_levels[0]);
    wbGameDraw(&game->shader, game->graphic.sprite_atlas_texture_id,
    WB_GRAPHIC_SPRITE_WIDTH_X , offset_x,
    WB_GRAPHIC_SPRITE_HEIGHT_Y, 0.0,
    WB_GRAPHIC_SPRITE_WIDTH_U , WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_X / WB_GRAPHIC_SPRITE_ATLAS_WIDTH,
    WB_GRAPHIC_SPRITE_HEIGHT_V, WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_Y / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT);
    glUniform1i(game->shader.loc.key_color_mode, WB_COLORMODE_CYCLE);
}

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color
    
    glUniform1f(game->shader.loc.time, game->gamestate.time);
    glUniform1f(game->shader.loc.window_scale, WB_GRAPHIC_WINDOW_HEIGHT / game->window.height);

    // entity sprite height is centered in atlas. They get shifted up by half a pixel with + 2.0 in offset_y
    switch (game->gamestate.state) {
        case WB_GAMESTATE_TITLESCREEN:
        glUniform1f(game->shader.loc.replace_color_mirror_height, WB_GRAPHIC_WINDOW_HEIGHT - WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y + 1);
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
        wbGameDrawMap(game, game->graphic.background_atlas_texture_id);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWizSpawn(game);
        wbGameDrawMap(game, game->graphic.foreground_atlas_texture_id);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_PLAY:
        wbGameDrawDust(game);
        wbGameDrawMap(game, game->graphic.background_atlas_texture_id);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWiz(game);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_DEATH:
        wbGameDrawDust(game);
        wbGameDrawMap(game, game->graphic.background_atlas_texture_id);
        wbGameDrawEntities(game);
        wbGameDrawPlayerCat(game);
        wbGameDrawGui(game);
        break;

        case WB_GAMESTATE_GAMEOVER:
        wbGameDrawDust(game);
        wbGameDrawMap(game, game->graphic.background_atlas_texture_id);
        wbGameDrawGui(game);
        wbGameDrawText(game, "GAME OVER", WB_TEXT_LARGE, 1.0f, 1.0f, 0,
            0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_GAMEOVER_OFFSET_Y) / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        wbGameDrawText(game, "player one", WB_TEXT_SMALL, 1.0f, 1.0f, 0,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.blue8, WB_GRAPHIC_COLORMAP_RGB8_CNT, WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
        break;
    }

    // toggle fps (not in real game)
    static bool show_fps = false;
    static bool key_fps_alt_pref = false;
    static float fps_filtered = 0.0f;
    float fps = 1.0 / game->gamestate.delta_time;
    float fps_filter_const = game->gamestate.delta_time / WB_GRAPHIC_GUI_FPS_LOWPASS_TIMECONST;
    fps_filtered = (fps_filter_const * fps + fps_filtered) / (fps_filter_const + 1);
    bool key_fps_alt = glfwGetKey(game->window.handle, WB_KEY_ALT_FPS)
                   && (glfwGetKey(game->window.handle, WB_KEY_ALTL) || glfwGetKey(game->window.handle, WB_KEY_ALTR));
    show_fps ^= key_fps_alt && !key_fps_alt_pref;
    if (show_fps) {
        sprintf(game->graphic.text, "%04i    ", (int)round(fps_filtered));
        wbGameDrawText(game, game->graphic.text, WB_TEXT_DIGIT, 1.0, 1.0, 0,
            1.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT,
            game->graphic.colormap.green8, 6, WB_GRAPHIC_GUI_COLORMAP_SPEED, WB_COLORMODE_CYCLE);
    }
    key_fps_alt_pref = key_fps_alt;

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
        static double time_prev = 0.0;
        double time = glfwGetTime();
        game.gamestate.delta_time = fmin(time - time_prev, 1.0 / WB_FPS_MIN);
        uint64_t time_frame = (uint64_t)round(glfwGetTime() * WB_FPS_MAX);
        static uint64_t time_frame_prev;
        // Poll for and process events
        // This checks for input events (keyboard, mouse) and window events (resize, close)
        glfwPollEvents();

        // Run with fps
        if (time_frame == time_frame_prev) continue;
        if (glfwGetKey(game.window.handle, WB_KEY_SLOWDOWN)) game.gamestate.delta_time *= 1.0f / 25; /*TODO: for debug*/
        if (glfwGetKey(game.window.handle, WB_KEY_SPEEDUP)) game.gamestate.delta_time *= 5; /*TODO: for debug*/

        wbWindowLockAspectRatio(&game.window);

        // --- Fullscreen toggle on Alt+Enter ---
        static bool key_fullscreen_alt_pref = false;
        bool key_alt = glfwGetKey(game.window.handle, WB_KEY_ALTL) || glfwGetKey(game.window.handle, WB_KEY_ALTR);
        bool key_fullscreen_alt = glfwGetKey(game.window.handle, WB_KEY_ALT_FULLSCREEN) && key_alt;
        if (key_fullscreen_alt && !key_fullscreen_alt_pref) {
            wbWindowToggleFullscreen(&game.window);
            wbWindowLockAspectRatio(&game.window);
        }
        key_fullscreen_alt_pref = key_fullscreen_alt;
        // --- End fullscreen toggle ---

        // --- VSync toggle on Alt+V ---
        static bool key_vsync_alt_prev = false;
        bool key_vsync_alt = glfwGetKey(game.window.handle, WB_KEY_ALT_VSYNC) && key_alt;
        if (key_vsync_alt && !key_vsync_alt_prev) {
            game.window.vsync ^= 1;
            glfwSwapInterval(game.window.vsync);
        }
        key_vsync_alt_prev = key_vsync_alt;
        // --- End VSync toggle ---

        static bool key_confirm_prev = false;
        bool key_confirm = glfwGetKey(game.window.handle, WB_KEY_CONFIRM_0) || glfwGetKey(game.window.handle, WB_KEY_CONFIRM_1);
        key_confirm &= !key_alt;

        switch (game.gamestate.state) {
            case WB_GAMESTATE_TITLESCREEN: // TODO: also switch to other titlescreens (player select, wiztips, ...)
            wbGameRender(&game);
            if (key_confirm && !key_confirm_prev) {
                wbGamestateSetupGetready(&game.gamestate, &game.sound, &game.map.view, &game.enemy_buffer, &game.particle_buffer, &game.projectile_buffer);
            }
            break;

            case WB_GAMESTATE_GETREADY:
            uint32_t seed = glfwGetTime() * 1e9;
            if (randfin(seed, 0.0f, 1.0f) < 0.0625f * game.gamestate.delta_time * 50 || !game.gamestate.time) {
                wbEnemyInsertRandoms(&game.enemy_buffer, game.gamestate.time);
            }
            wbGameRender(&game);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            if (key_confirm && !key_confirm_prev) {
                ma_sound_stop(&game.sound.getready);
                game.gamestate.powerup.slot = -1;
                game.gamestate.powerup.unlocked = game.gamestate.powerup.permanent;
                int pos_x_min = WB_GRAPHIC_MAP_VIEW_WIDTH / 2 - 1;
                int pos_x_max = WB_GRAPHIC_MAP_ATLAS_WIDTH - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + 1;
                wbPlayerWizInit(&game.player.wiz, pos_x_min, pos_x_max);
                wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
                wbPlayerCatInit(&game.player.cat);
                wbBufferClear(&game.enemy_buffer);
                wbEnemyPopulate(&game.enemy_buffer, WB_ENEMY_SPINNERCYAN, WB_GRAPHIC_ENEMY_COLORMAP_CYAN_OFFSET, WB_MOVEPATTERN_INERT, &game.map.view);
                game.gamestate.time = -game.gamestate.delta_time;
                game.gamestate.state = WB_GAMESTATE_SPAWN;
            }
            break;

            case WB_GAMESTATE_SPAWN:
            wbGameProcessInput(&game);
            wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate);
            wbParticleUpdate(&game.particle_buffer, &game.player, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.map, &game.player, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            if (game.gamestate.time * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED < WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_FRAME_CNT) {
                wbGameRender(&game);
            } else {
                game.player.wiz.animation_angle = 0.0f;
                game.gamestate.state = WB_GAMESTATE_PLAY;
                wbGameRender(&game);
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
                game.gamestate.time = -game.gamestate.delta_time;
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
                    game.gamestate.time = -game.gamestate.delta_time;
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

        key_confirm_prev = key_confirm;

        time_frame_prev = time_frame;
        time_prev = time;
        game.gamestate.time += game.gamestate.delta_time;
    }

    wbGameUninit(&game);
    return 0;
}
