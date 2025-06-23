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

    // Initialize graphic
    WBGraphic* graphic = &game->graphic;
    graphic->enemy_animation_colors[0] = WB_GRAPHIC_ENEMY_ANIMATION_COLOR_0;
    graphic->enemy_animation_colors[1] = WB_GRAPHIC_ENEMY_ANIMATION_COLOR_1;
    graphic->enemy_animation_colors[2] = WB_GRAPHIC_ENEMY_ANIMATION_COLOR_2;
    graphic->enemy_animation_colors[3] = WB_GRAPHIC_ENEMY_ANIMATION_COLOR_3;
    graphic->projectile_animation_colors[0] = WB_GRAPHIC_PROJECTILE_ANIMATION_COLOR_0;
    graphic->projectile_animation_colors[1] = WB_GRAPHIC_PROJECTILE_ANIMATION_COLOR_1;
    graphic->projectile_animation_colors[2] = WB_GRAPHIC_PROJECTILE_ANIMATION_COLOR_2;
    graphic->projectile_animation_colors[3] = WB_GRAPHIC_PROJECTILE_ANIMATION_COLOR_3;
    graphic->projectile_animation_colors[4] = WB_GRAPHIC_PROJECTILE_ANIMATION_COLOR_4;
    graphic->gui_red_animation_colors[0] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_0;
    graphic->gui_red_animation_colors[1] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_1;
    graphic->gui_red_animation_colors[2] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_2;
    graphic->gui_red_animation_colors[3] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_3;
    graphic->gui_red_animation_colors[4] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_4;
    graphic->gui_red_animation_colors[5] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_5;
    graphic->gui_red_animation_colors[6] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_6;
    graphic->gui_red_animation_colors[7] = WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_7;
    graphic->gui_green_animation_colors[0] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_0;
    graphic->gui_green_animation_colors[1] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_1;
    graphic->gui_green_animation_colors[2] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_2;
    graphic->gui_green_animation_colors[3] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_3;
    graphic->gui_green_animation_colors[4] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_4;
    graphic->gui_green_animation_colors[5] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_5;
    graphic->gui_green_animation_colors[6] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_6;
    graphic->gui_green_animation_colors[7] = WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_7;
    graphic->gui_blue_animation_colors[0] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_0;
    graphic->gui_blue_animation_colors[1] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_1;
    graphic->gui_blue_animation_colors[2] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_2;
    graphic->gui_blue_animation_colors[3] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_3;
    graphic->gui_blue_animation_colors[4] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_4;
    graphic->gui_blue_animation_colors[5] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_5;
    graphic->gui_blue_animation_colors[6] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_6;
    graphic->gui_blue_animation_colors[7] = WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_7;
    graphic->gui_powerup_animation_colors[0] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_0;
    graphic->gui_powerup_animation_colors[1] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_1;
    graphic->gui_powerup_animation_colors[2] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_2;
    graphic->gui_powerup_animation_colors[3] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_3;
    graphic->gui_powerup_animation_colors[4] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_4;
    graphic->gui_powerup_animation_colors[5] = WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_5;
    graphic->text_wizball_animation_colors[ 0] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_0;
    graphic->text_wizball_animation_colors[ 1] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_1;
    graphic->text_wizball_animation_colors[ 2] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_2;
    graphic->text_wizball_animation_colors[ 3] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_3;
    graphic->text_wizball_animation_colors[ 4] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_4;
    graphic->text_wizball_animation_colors[ 5] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_5;
    graphic->text_wizball_animation_colors[ 6] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_6;
    graphic->text_wizball_animation_colors[ 7] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_7;
    graphic->text_wizball_animation_colors[ 8] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_8;
    graphic->text_wizball_animation_colors[ 9] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_9;
    graphic->text_wizball_animation_colors[10] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_10;
    graphic->text_wizball_animation_colors[11] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_11;
    graphic->text_wizball_animation_colors[12] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_12;
    graphic->text_wizball_animation_colors[13] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_13;
    graphic->text_wizball_animation_colors[14] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_14;
    graphic->text_wizball_animation_colors[15] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_15;
    graphic->text_wizball_animation_colors[16] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_16;
    graphic->text_wizball_animation_colors[17] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_17;
    graphic->text_wizball_animation_colors[18] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_18;
    graphic->text_wizball_animation_colors[19] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_19;
    graphic->text_wizball_animation_colors[20] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_20;
    graphic->text_wizball_animation_colors[21] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_21;
    graphic->text_wizball_animation_colors[22] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_22;
    graphic->text_wizball_animation_colors[23] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_23;
    graphic->text_wizball_animation_colors[24] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_24;
    graphic->text_wizball_animation_colors[25] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_25;
    graphic->text_wizball_animation_colors[26] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_26;
    graphic->text_wizball_animation_colors[27] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_27;
    graphic->text_wizball_animation_colors[28] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_28;
    graphic->text_wizball_animation_colors[29] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_29;
    graphic->text_wizball_animation_colors[30] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_30;
    graphic->text_wizball_animation_colors[31] = WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_31;
    graphic->text_highscore_animation_colors[0] = WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_0;
    graphic->text_highscore_animation_colors[1] = WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_1;
    graphic->text_highscore_animation_colors[2] = WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_2;
    graphic->text_highscore_animation_colors[3] = WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_3;
    graphic->text_topscores_animation_colors[0] = WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_0;
    graphic->text_topscores_animation_colors[1] = WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_1;
    graphic->text_topscores_animation_colors[2] = WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_2;
    graphic->text_topscores_animation_colors[3] = WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_3;
    graphic->text_scoreboard_animation_colors[0] = WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_0;
    graphic->text_scoreboard_animation_colors[1] = WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_1;
    graphic->text_scoreboard_animation_colors[2] = WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_2;
    graphic->text_scoreboard_animation_colors[3] = WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_3;

    // Initialize sound
    if (ma_engine_init(NULL, &game->sound.engine) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize sound engine\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_TITLESCREEN_PATH, MA_SOUND_FLAG_LOOPING, NULL, NULL, &game->sound.titlescreen) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load titlescreen sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_GETREADY_PATH, 0, NULL, NULL, &game->sound.getready) != MA_SUCCESS) {
        fprintf(stderr, "Failed to getready sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_FIRE_PATH, 0, NULL, NULL, &game->sound.fire) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load fire sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_FIRE_SPAM_PATH, 0, NULL, NULL, &game->sound.fire_spam) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load fire spam sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_POWERUP_DROP_PATH, 0, NULL, NULL, &game->sound.powerup_drop) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load powerup drop sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_POWERUP_COLLECT_PATH, 0, NULL, NULL, &game->sound.powerup_collect) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load powerup collect sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_POWERUP_ACTIVATE_PATH, 0, NULL, NULL, &game->sound.powerup_activate) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load powerup activate sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_DECAY_PATH, 0, NULL, NULL, &game->sound.decay) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load decay sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_CLEAR_PATH, 0, NULL, NULL, &game->sound.clear) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load clear sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_BLINKER_PATH, 0, NULL, NULL, &game->sound.blinker) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load blinker sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_BOMB_PATH, 0, NULL, NULL, &game->sound.bomb) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load bomb sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_CATHIT_PATH, 0, NULL, NULL, &game->sound.cathit) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load cathit sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_WIZDEATH_PATH, 0, NULL, NULL, &game->sound.wizdeath) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load wizdeath sound\n");
        return false;
    }
    if (ma_sound_init_from_file(&game->sound.engine, WB_SOUND_GAMEOVER_PATH, 0, NULL, NULL, &game->sound.gameover) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load gameover sound\n");
        return false;
    }

    // Initialize gamestate
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

    game->last_frame_time = 0;
    game->frame_cnt = 0;

    return true;
}

void wbGameUninit(WBGame* game) {
    // Cleanup resources
    glDeleteVertexArrays(1, &game->shader.vao);
    glDeleteBuffers(1, &game->shader.vbo);
    glDeleteBuffers(1, &game->shader.ebo);
    glDeleteProgram(game->shader.program);
    glDeleteTextures(1, &game->sprite_atlas.texture_id); // Delete the sprite atlas texture
    glDeleteTextures(1, &game->map.atlas.background.texture_id);
    glDeleteTextures(1, &game->map.atlas.collider_texture.texture_id);
    glDeleteTextures(1, &game->map.atlas.dust.texture_id);
    free(game->map.atlas.collider); // Free the map atlas collider memory
    ma_sound_uninit(&game->sound.gameover);
    ma_sound_uninit(&game->sound.wizdeath);
    ma_sound_uninit(&game->sound.cathit);
    ma_sound_uninit(&game->sound.bomb);
    ma_sound_uninit(&game->sound.blinker);
    ma_sound_uninit(&game->sound.clear);
    ma_sound_uninit(&game->sound.decay);
    ma_sound_uninit(&game->sound.powerup_activate);
    ma_sound_uninit(&game->sound.powerup_collect);
    ma_sound_uninit(&game->sound.powerup_drop);
    ma_sound_uninit(&game->sound.fire_spam);
    ma_sound_uninit(&game->sound.fire);
    ma_sound_uninit(&game->sound.getready);
    ma_sound_uninit(&game->sound.titlescreen);
    ma_engine_uninit(&game->sound.engine);
    glfwDestroyWindow(game->window.handle); // Destroy the window
    glfwTerminate(); // Terminate GLFW to clean up resources
}

void wbGameProcessInput(WBGame* game) {
    if (game->frame_cnt % WB_GAMERULE_PROCESS_INPUT_FRAME_CNT) {
        return;
    }
    WBMap* map = &game->map;
    
    static int wiggle_cnt = 0;
    static double wiggle_frame = 0;
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

    static uint64_t left_down_frame;
    if (wiz_left && !game->window.prev_key_state[WB_KEY_WIZ_LEFT]) {
        left_down_frame = game->frame_cnt;
    }
    left_down_frame *= wiz_left;
    static uint64_t right_down_frame;
    if (wiz_right && !game->window.prev_key_state[WB_KEY_WIZ_RIGHT]) {
        right_down_frame = game->frame_cnt;
    }
    right_down_frame *= wiz_right;
    if (!wiggle_cnt) {
        wiggle_frame = game->frame_cnt;
    }
    WBDirectionType dir = left_down_frame > 0 || right_down_frame > 0 ?
                          left_down_frame > right_down_frame ? WB_DIRECTION_NEGATIVE : WB_DIRECTION_POSITIVE : 0;
    if (dir != 0 && dir != wiggle_dir) {
        if (game->frame_cnt - wiggle_frame < 1.0f / WB_GAMERULE_POWERUP_WIGGLE_SPEED) {
            wiggle_dir *= -1;
            wiggle_frame = game->frame_cnt;
            wiggle_cnt++;
        }
        else {
            wiggle_cnt = 0;
        }
    }
    bool activate_powerup = glfwGetKey(game->window.handle, WB_KEY_POWERUP);
    activate_powerup |= wiggle_cnt >= WB_GAMERULE_POWERUP_WIGGLE_CNT;
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
    static uint64_t wiz_fire_down_frame;
    if (wiz_fire && !game->window.prev_key_state[WB_KEY_WIZ_FIRE]) {
        wiz_fire_down_frame = game->frame_cnt;
    }
    wiz_fire_down_frame *= wiz_fire;
    bool autofire = wiz_fire_down_frame > 0 &&
                    game->frame_cnt - wiz_fire_down_frame >= WB_GAMERULE_AUTOFIRE_FRAME_CNT &&
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
        wiz->vel_x_key += WB_GAMERULE_PLAYER_WIZ_ACC_X * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT;
        wiz->vel_x_key = fminf(wiz->vel_x_key,   WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint);
    }
    if (wiz_left) {
        wiz->vel_x_key -= WB_GAMERULE_PLAYER_WIZ_ACC_X * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT;
        wiz->vel_x_key = fmaxf(wiz->vel_x_key, -(WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT - 1 - !sprint));
    }

    float vel_x, vel_y;
    if (powerup->unlocked & WB_POWERUP_ANTIGRAV) {
        if (wiz_down) {
            wiz->vel_y_key += WB_GAMERULE_PLAYER_WIZ_ACC_Y * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT;
            wiz->vel_y_key = fminf(wiz->vel_y_key,  WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT - 1);
        }
        if (wiz_up) {
            wiz->vel_y_key -= WB_GAMERULE_PLAYER_WIZ_ACC_Y * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT;
            wiz->vel_y_key = fmaxf(wiz->vel_y_key, -WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT + 1);
        }
        vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
        wiz->vel_x_key -= fsgnf(vel_x) * WB_GAMERULE_PLAYER_WIZ_DEC_X * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT * (!wiz_left && !wiz_right);
        vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
        wiz->vel_y_key -= fsgnf(vel_y) * WB_GAMERULE_PLAYER_WIZ_DEC_Y * WB_GAMERULE_PROCESS_INPUT_FRAME_CNT * (!wiz_up && !wiz_down);
    }

    static uint64_t last_fire_frame = 0;
    if ((wiz_fire && !game->window.prev_key_state[WB_KEY_WIZ_FIRE] || autofire) && 
        (game->gamestate.state == WB_GAMESTATE_PLAY || game->gamestate.state == WB_GAMESTATE_SPAWN)) {
        WBVec2f vel;
        if (map->view.bullet_wiz_cnt < WB_GAMERULE_MAP_VIEW_BULLET_WIZ_CNT_MAX) {
            if (game->frame_cnt - last_fire_frame == WB_GAMERULE_PROCESS_INPUT_FRAME_CNT) {
                ma_sound_seek_to_pcm_frame(&game->sound.fire_spam, 0);
                ma_sound_start(&game->sound.fire_spam);
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
            last_fire_frame = game->frame_cnt;
        }

        if (!map->view.spray && powerup->unlocked & WB_POWERUP_WIZSPRAY) {
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
            map->view.spray = true;
        }

        if (!map->view.beam && powerup->unlocked & (WB_POWERUP_BEAM | WB_POWERUP_DOUBLE)) {
            int idx = wbProjectileAppend(&game->projectile_buffer, WB_PROJECTILE_BEAM, &wiz->pos, &vel);
            game->projectile_buffer.entries[idx].head.color_key = randfin(time(NULL), 0, WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_COLOR_CNT);
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

    float prev_cat_pos_x = cat->pos.x;

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
        if (map->view.bullet_cat_cnt < WB_GAMERULE_MAP_VIEW_BULLET_CAT_CNT_MAX) {
            if (game->frame_cnt - last_fire_frame == WB_GAMERULE_PROCESS_INPUT_FRAME_CNT) {
                ma_sound_seek_to_pcm_frame(&game->sound.fire_spam, 0);
                ma_sound_start(&game->sound.fire_spam);
            } else {
                ma_sound_seek_to_pcm_frame(&game->sound.fire, 0);
                ma_sound_start(&game->sound.fire);
            }
            vel.x = WB_GAMERULE_PROJECTILE_VEL * cat->facing;
            vel.y = 0.0f;
            WBProjectileType type = powerup->unlocked & WB_POWERUP_BLAZERS ? WB_PROJECTILE_BLAZER_CAT : WB_PROJECTILE_BULLET_CAT;
            wbProjectileAppend(&game->projectile_buffer, type, &cat->pos, &vel);
            last_fire_frame = game->frame_cnt;
        }
        if (!map->view.spray && powerup->unlocked & WB_POWERUP_CATSPRAY) {
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
            map->view.spray = true;
        }
    }
    game->window.prev_key_state[WB_KEY_WIZ_FIRE] = wiz_fire;
    game->window.prev_key_state[WB_KEY_CAT_FIRE] = cat_fire;
}

void wbGameDraw(WBShader* shader, GLuint texture_id,
    float width_x, float offset_x, float height_y, float offset_y, float width_u, float offset_u, float height_v, float offset_v) {
    // x, y, u, v
    shader->vertices[ 0] = -width_x + offset_x; shader->vertices[ 1] = -1.0f + 2.0f * height_y + offset_y; shader->vertices[ 2] = offset_u          ; shader->vertices[ 3] = offset_v           ;
    shader->vertices[ 4] =  width_x + offset_x; shader->vertices[ 5] = -1.0f + 2.0f * height_y + offset_y; shader->vertices[ 6] = width_u + offset_u; shader->vertices[ 7] = offset_v           ;
    shader->vertices[ 8] =  width_x + offset_x; shader->vertices[ 9] = -1.0f                   + offset_y; shader->vertices[10] = width_u + offset_u; shader->vertices[11] = offset_v + height_v;
    shader->vertices[12] = -width_x + offset_x; shader->vertices[13] = -1.0f                   + offset_y; shader->vertices[14] =           offset_u; shader->vertices[15] = offset_v + height_v;

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shader->vertices), shader->vertices);
    glDrawElements(GL_TRIANGLES, WB_GRAPHIC_SPRITE_INDICES_CNT, GL_UNSIGNED_INT, 0); // Draw quad
}

void wbGameDrawDust(WBGame* game, GLint key_alpha_loc) {
    WBMap* map = &game->map;
    float map_view_width = WB_GRAPHIC_MAP_VIEW_WIDTH;
    float map_height = (float)map->atlas.background.height / WB_MAP_CNT;
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float dust_sprite_size = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE;
    float dust_sprite_width = dust_sprite_size * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X;
    float dust_sprite_height = dust_sprite_size * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y;
    float width_x = dust_sprite_width / window_width;
    float offset_x;
    float height_y = dust_sprite_height / window_height;
    float offset_y;
    float width_u = dust_sprite_size / map->atlas.dust.width;
    float offset_u;
    float height_v = dust_sprite_size / map->atlas.dust.height;
    float offset_v = 0.0f;
    int dust_row_cnt = ceil(WB_GAMERULE_MAP_HORIZON_HEIGHT / dust_sprite_height);
    int dust_col_cnt = ceil(WB_GRAPHIC_MAP_VIEW_WIDTH / dust_sprite_width) + 1;
    for (int layer = 0; layer < WB_GRAPHIC_MAP_DUST_LAYER_CNT; layer++) {
        for (int row = 0; row < dust_row_cnt; row++) {
            for (int col = 0; col < dust_col_cnt; col++) {
                offset_x = (-2.0f * roundf(map->view.center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT
                            -2.0f * roundf(WB_GRAPHIC_MAP_DUST_VELOCITY_FACTOR * layer * map->view.center_x / WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT)
                           / window_width + width_x + 2.0f * col * width_x;
                offset_x = fmodf(offset_x, 2.0f * dust_col_cnt * width_x);
                offset_x += (offset_x < -dust_col_cnt * width_x) ? 2.0f * dust_col_cnt * width_x : 0.0f;
                offset_x += (row / (dust_row_cnt - 1.0f) - 0.5f) * width_x;
                offset_y = 2.0f * (WB_GRAPHIC_MAP_VIEW_OFFSET_Y + map_height) / window_height - 2.0f * height_y - 2.0f * row * height_y;
                offset_u = layer * dust_sprite_size / map->atlas.dust.width;
                wbGameDraw(&game->shader, map->atlas.dust.texture_id,
                    width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
                );
            }
        }
        glUniform1f(key_alpha_loc, 128.0/255.0);
    }
    glUniform1f(key_alpha_loc, -1.0);
}

void wbGameDrawMap(WBGame* game, WBTexture* texture) {
    WBMap* map = &game->map;
    float map_view_width = WB_GRAPHIC_MAP_VIEW_WIDTH;
    float map_height = (float)texture->height / WB_MAP_CNT;
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float width_x = map_view_width / window_width;
    float offset_x = 0.0f;
    float height_y = map_height / window_height;
    float offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height;
    float width_u = map_view_width / texture->width;
    float offset_u = roundf(map->view.center_x / WB_GRAPHIC_MAP_SUBPIXEL_CNT) * WB_GRAPHIC_MAP_SUBPIXEL_CNT / texture->width - 0.5f * width_u;
    float height_v = 1.0f / WB_MAP_CNT;
    float offset_v = (float)game->gamestate.level / WB_MAP_CNT;
    wbGameDraw(&game->shader, texture->texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );
}

void wbGameDrawEntities(WBGame* game, GLint replace_colors_loc) {
    WBProjectile* projectile;
    WBParticle* particle;
    WBEnemy* enemy;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->map.atlas.background.height / WB_MAP_CNT;
    float rgba[4];

    // particles
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
            uint32_t color = game->graphic.enemy_animation_colors[(int)particle->head.color_key];
            ui32to4f(rgba, color);
            glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
            break;
        }
        offset_x = 2.0f * roundf((particle->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(particle->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // enemies
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &game->enemy_buffer.entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERBLUE:
            offset_u = WB_GRAPHIC_ENEMY_SPINNERBLUE_SPRITE_ATLAS_X / sprite_atlas_width + (int)enemy->head.animation_key * width_u;
            offset_v = WB_GRAPHIC_ENEMY_SPINNERBLUE_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;

            case WB_ENEMY_CIRCLE:
            offset_u = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_X / sprite_atlas_width;
            offset_v = WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_Y / sprite_atlas_height;
            uint32_t color = game->graphic.enemy_animation_colors[(int)enemy->head.color_key];
            ui32to4f(rgba, color);
            glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
            break;
        }
        offset_x = 2.0f * roundf((enemy->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(enemy->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // projectiles
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
                uint32_t color = game->graphic.projectile_animation_colors[color_key];
                ui32to4f(rgba, color);
                glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
                projectile->head.pos.y -= 0.5f * WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
                offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                         +(2.0f * map_height - sprite_size + 2.0f) / window_height
                         - 2.0f * roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / sprite_atlas_width + (int)projectile->head.animation_key * width_u;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / sprite_atlas_height;
                wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
                    width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
                );
                projectile->head.pos.y += WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y;
                offset_u = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X / sprite_atlas_width + ((int)projectile->head.animation_key + WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT) * width_u;
                offset_v = WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y / sprite_atlas_height;
            break;
        }
        offset_x = 2.0f * roundf((projectile->head.pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
        offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
                 +(2.0f * map_height - sprite_size + 2.0f) / window_height
                 - 2.0f * roundf(projectile->head.pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }
}

void wbGameDrawPlayerCat(WBGame* game) {
    WBCat* cat = &game->player.cat;
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->map.atlas.background.height / WB_MAP_CNT;

    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width
             + 2.0f * roundf((cat->pos.x - wiz->pos.x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(cat->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_X / sprite_atlas_width
             + (uint64_t)((double)game->frame_cnt * WB_GRAPHIC_PLAYER_CAT_ANIMATION_SPEED) % WB_GRAPHIC_PLAYER_CAT_ANIMATION_FRAME_CNT * width_u;
    offset_v = WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );
}

void wbGameDrawPlayerWiz(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->map.atlas.background.height / WB_MAP_CNT;

    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = roundf(wiz->animation_angle) * width_u + WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_X / sprite_atlas_width;
    offset_v = WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );
}

void wbGameDrawPlayerWizSpawn(WBGame* game) {
    WBWiz* wiz = &game->player.wiz;

    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float map_height = (float)game->map.atlas.background.height / WB_MAP_CNT;

    // player wiz
    offset_x = 2.0f * roundf((wiz->pos.x - game->map.view.center_x) / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_width;
    offset_y = 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / window_height
             +(2.0f * map_height - sprite_size + 2.0f) / window_height
             - 2.0f * roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT / window_height;
    offset_u = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_X / sprite_atlas_width
             + ((uint64_t)((double)game->gamestate.frame_counter * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) % (game->sprite_atlas.width / WB_GRAPHIC_SPRITE_SIZE)) * width_u;
    offset_v = WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_Y / sprite_atlas_height
             + ((uint64_t)((double)game->gamestate.frame_counter * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED) / (game->sprite_atlas.width / WB_GRAPHIC_SPRITE_SIZE)) * height_v;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );
}

void wbGameDrawGui(WBGame* game, GLint replace_colors_loc) {
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;
    float sprite_size = WB_GRAPHIC_SPRITE_SIZE;
    float width_x = sprite_size / window_width;
    float height_y = sprite_size / window_height;
    float width_u = sprite_size / sprite_atlas_width;
    float height_v = sprite_size / sprite_atlas_height;
    float offset_x, offset_y, offset_u, offset_v;
    float rgba[4];

    // powerup slots
    for (int i = 0; i < WB_POWERUP_SLOT_CNT; i++) {
        int animation_color_idx = i == game->gamestate.powerup.slot ? (uint64_t)((double)game->frame_cnt * WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_SPEED) % WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_CNT : 1;
        uint32_t color = game->graphic.gui_powerup_animation_colors[animation_color_idx];
        ui32to4f(rgba, color);
        glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
        offset_x = -1.0f + (window_width - (WB_POWERUP_SLOT_CNT - 1) * WB_GRAPHIC_GUI_POWERUP_STRIDE) / window_width + 2.0f * i * WB_GRAPHIC_GUI_POWERUP_STRIDE / window_width;
        offset_y = 2.0f - 2.0f * WB_GRAPHIC_GUI_POWERUP_STRIDE / window_height;
        int powerup_slotstate = ((game->gamestate.powerup.unlocked >> 2 * i) & WB_POWERUP_SLOTMASK);
        if (i == 4) {
            powerup_slotstate &= 1;
        }
        if (powerup_slotstate < 2) {
            offset_u = ((2.0f * i + powerup_slotstate) * sprite_size + WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X) / sprite_atlas_width;
        }
        else {
            offset_u = WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X / sprite_atlas_width;
        }
        offset_v = WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_Y / sprite_atlas_height;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
    }

    // 1UP
    int animation_color_idx = (uint64_t)((double)game->frame_cnt * WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED) % WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT;
    uint32_t color = game->graphic.gui_red_animation_colors[animation_color_idx];
    ui32to4f(rgba, color);
    glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
    width_x = 3.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    offset_x = -1.0f + 3.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    height_y = WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_height;
    offset_y = 2.0f - 2.0f * WB_GRAPHIC_GUI_SCORE_OFFSET_Y / window_height;
    width_u = 3.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_width;
    offset_u = WB_GRAPHIC_TEXT_DIGIT_1UP_SPRITE_ATLAS_X / sprite_atlas_width;
    height_v = WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_height;
    offset_v = WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y / sprite_atlas_height;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );

    // score
    width_x = WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    offset_x = -1.0f + WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    offset_y = 2.0f - 2.0f * (WB_GRAPHIC_GUI_SCORE_OFFSET_Y + WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / window_height;
    width_u = WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_width;
    offset_v = WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y / sprite_atlas_height;
    char str[7];
    sprintf(str, "%06i", game->gamestate.score);
    for (char* p = str; *p; p++) {
        offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + (*p - '0') * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
        offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    }

    // lifes
    color = game->graphic.gui_green_animation_colors[(animation_color_idx - 2 + WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT) % WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT];
    ui32to4f(rgba, color);
    glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
    offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    sprintf(str, "%01i", game->gamestate.lifes ? game->gamestate.lifes - 1 : 0);
    offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + (*str - '0') * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );
    offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    // enemy_cnt
    color = game->graphic.gui_blue_animation_colors[(animation_color_idx - 1 + WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT) % WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT];
    ui32to4f(rgba, color);
    glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
    offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    sprintf(str, "%02i", game->gamestate.enemy_cnt);
    for (char* p = str; *p; p++) {
        offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + (*p - '0') * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
        );
        offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    }

    // HI

    // highscore

    // 2UP

    // score2

    // level
    uint32_t* animation_colors = (uint32_t*)((uint8_t*)game->graphic.gui_red_animation_colors + game->gamestate.level % 3 * (sizeof game->graphic.gui_red_animation_colors));
    animation_color_idx = (uint64_t)((double)game->frame_cnt * WB_GRAPHIC_GUI_LEVEL_ANIMATION_COLOR_SPEED) % WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT;
    color = animation_colors[animation_color_idx];
    ui32to4f(rgba, color);
    glUniform4fv(replace_colors_loc, 1, rgba); // #FF00FFFF
    width_x = 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width;
    offset_x = -1.0f + 2.0f * WB_GRAPHIC_GUI_LEVEL_OFFSET_X / window_width;
    height_y = 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_height;
    offset_y = 2.0f * WB_GRAPHIC_GUI_LEVEL_OFFSET_Y / window_height;
    sprintf(str, "%01i", game->gamestate.level + 1);
    offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + (*str - '0') * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
    wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
        width_x, offset_x, height_y, offset_y, width_u, offset_u, height_v, offset_v
    );

    // cauldrons

}

void wbGameDrawText(WBGame* game, char* text, float width_scale, uint64_t draw_gamestate_frame,
    float offset_x, float offset_y,
    GLint key_color_mode_loc, int color_mode,
    GLint replace_color_speed_loc, float color_speed,
    GLint replace_color_cnt_loc, int color_cnt_lim, int color_cnt,
    GLint replace_colors_loc, uint32_t* colors) {
    
    if (game->gamestate.frame_counter < draw_gamestate_frame) return;
    
    float window_width = WB_GRAPHIC_WINDOW_WIDTH;
    float window_height = WB_GRAPHIC_WINDOW_HEIGHT;
    float sprite_atlas_width = game->sprite_atlas.width;
    float sprite_atlas_height = game->sprite_atlas.height;

    static float replace_colors[WB_GRAPHIC_TEXT_ANIMATION_COLOR_CNT_MAX * 4];
    if (color_mode == 1) {
        for (int i = 0; i < color_cnt; i++) {
            uint32_t color = i >= color_cnt_lim ? 0x000000FF : colors[i];
            int idx = (-i + color_cnt) % color_cnt;
            ui32to4f(&replace_colors[idx * 4], color);
        }
        glUniform1f(replace_color_speed_loc, color_speed);
        glUniform1i(replace_color_cnt_loc, color_cnt);
    } else {
        int idx = (uint64_t)((double)(game->gamestate.frame_counter - draw_gamestate_frame) * color_speed) % color_cnt;
        ui32to4f(replace_colors, colors[idx]);
        color_cnt = 1;
    }
    glUniform1i(key_color_mode_loc, color_mode);
    glUniform4fv(replace_colors_loc, color_cnt, replace_colors);
    if (strcmp(text, "WIZBALL") == 0) {
        wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH  / window_width,  offset_x,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / window_height, offset_y,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH  / sprite_atlas_width,  WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_X / sprite_atlas_width,
            WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / sprite_atlas_height, WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_Y / sprite_atlas_height
        );
        glUniform1i(key_color_mode_loc, 0);
        return;
    }
    int i;
    float string_width;
    float _offset_x, offset_u;
    char* p = text;
    while (*p == ' ') { p++; }
    WBTextType char_type = '0' <= *p && *p <= '9' ?
        WB_TEXT_DIGIT : 'a' <= *p && *p <= 'z' || *p == '-' ?
        WB_TEXT_SMALL : 
        WB_TEXT_LARGE;
    switch (char_type) {
        case WB_TEXT_DIGIT:
        string_width = strlen(text);
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width * width_scale;
        for (p = text; *p; p++) {
            if (*p != ' ') {
                i = *p == '.' ? 10 : *p - '0';
                offset_u = (WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width * width_scale,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_height, offset_y,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y / sprite_atlas_height
                );
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE / window_width * width_scale;
        }
        break;
        case WB_TEXT_SMALL:
        string_width = strlen(text);
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width;
        for (p = text; *p; p++) {
            if (*p != ' ') {
                i = *p == '-' ? 26 : *p - 'a';
                offset_u = (WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_height, offset_y,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_Y / sprite_atlas_height
                );
            }
            _offset_x += 2.0f * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE / window_width;
        }
        break;
        case WB_TEXT_LARGE:
        string_width = 0.0f;
        for (p = text; *p; p++) {
            string_width += *p == 'I' || *p == 'L'? 0.5f : 1.0f;
        }
        _offset_x = (1.0f - string_width) * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width;
        for (p = text; *p; p++) {
            float char_w = *p == 'I' || *p == 'L' ? 0.5f : 1.0f;
            if (*p != ' ') {
                i = *p == '!' ? 26 : *p - 'A';
                offset_u = (WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_X + i * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE) / sprite_atlas_width;
                wbGameDraw(&game->shader, game->sprite_atlas.texture_id,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width,  offset_x + _offset_x,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_height, offset_y,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / sprite_atlas_width, offset_u,
                    WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / sprite_atlas_height, WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_Y / sprite_atlas_height
                );
            }
            _offset_x += 2.0f * char_w * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE / window_width;
        }
        break;
    }
}

void wbGameRender(WBGame* game) {
    // Set the clear color for the color buffer (RGBA values from 0.0 to 1.0)
    glClearColor(0.0, 0.0f, 0.0f, 1.0f); // black color
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the set clear color

    // Set the key color
    GLint key_color_loc = glGetUniformLocation(game->shader.program, "keyColor");
    glUniform4f(key_color_loc, 1.0f, 0.0f, 1.0f, 1.0f); // #FF00FFFF
    // Set the frame cnt
    GLint frame_counter_loc = glGetUniformLocation(game->shader.program, "frameCounter");
    glUniform1ui(frame_counter_loc, game->gamestate.frame_counter);
    // Set the delta time
    GLint delta_time_loc = glGetUniformLocation(game->shader.program, "deltaTime");
    glUniform1f(delta_time_loc, 1.0f / WB_FPS);
    // Set the key alpha
    GLint key_alpha_loc = glGetUniformLocation(game->shader.program, "keyAlpha");
    glUniform1f(key_alpha_loc, -1.0);
    // Set dust texture size
    GLint tex_size_loc = glGetUniformLocation(game->shader.program, "texSize");
    glUniform1f(tex_size_loc, WB_GRAPHIC_MAP_DUST_SPRITE_SIZE);
    // Set the key color mode
    GLint key_color_mode_loc = glGetUniformLocation(game->shader.program, "keyColorMode");
    glUniform1i(key_color_mode_loc, 0);
    // Set the window scale
    GLint window_scale_loc = glGetUniformLocation(game->shader.program, "windowScale");
    glUniform1f(window_scale_loc, (float)WB_GRAPHIC_WINDOW_HEIGHT / game->window.height);
    // Set the replace color reflect height
    GLint replace_color_reflect_height_loc = glGetUniformLocation(game->shader.program, "replaceColorReflectHeight");
    glUniform1f(replace_color_reflect_height_loc, WB_GRAPHIC_WINDOW_HEIGHT - WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y + 1);
    // Set the replace color height
    GLint replace_color_height_loc = glGetUniformLocation(game->shader.program, "replaceColorHeight");
    glUniform1f(replace_color_height_loc, WB_GRAPHIC_TEXT_COLOR_HEIGHT);
    // Set the replace color speed later
    GLint replace_color_speed_loc = glGetUniformLocation(game->shader.program, "replaceColorSpeed");
    // Set the replace color cnt later
    GLint replace_color_cnt_loc = glGetUniformLocation(game->shader.program, "replaceColorCnt");
    // Set the replace colors later
    GLint replace_colors_loc = glGetUniformLocation(game->shader.program, "replaceColors");
    // Set the subpixel cnt
    GLint subpixel_cnt_loc = glGetUniformLocation(game->shader.program, "subpixelCnt");
    glUniform1f(subpixel_cnt_loc, WB_GRAPHIC_SUBPIXEL_CNT);

    // entity sprite height is centered in atlas. They get shifted up by half a pixel with + 2.0 in offset_y
    switch (game->gamestate.state) {
        case WB_GAMESTATE_TITLESCREEN:
        static char text[32];
        int cnt = game->gamestate.frame_counter * WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT < WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT / 2 ?
                  game->gamestate.frame_counter * WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT + 1 : WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_CNT;
        wbGameDrawText(game, "WIZBALL", 1.0f, 0,
            0.0f, 2.0f - (2.0f * WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y + WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT) / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 1,
            replace_color_speed_loc, WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, cnt, WB_GRAPHIC_TEXT_WIZBALL_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.text_wizball_animation_colors
        );
        wbGameDrawText(game, "top scores", 1.0f, WB_GRAPHIC_TEXT_TOPSCORES_FRAME_CNT,
            0.0f, 2.0f - (2.0f * WB_GRAPHIC_TEXT_TOPSCORES_OFFSET_Y) / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_CNT, WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.text_topscores_animation_colors
        );
        glUniform1f(replace_color_reflect_height_loc, WB_GRAPHIC_WINDOW_HEIGHT);
        cnt = (game->gamestate.frame_counter - WB_GRAPHIC_TEXT_HIGHSCORE1_FRAME_CNT) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT < WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_CNT ?
              (game->gamestate.frame_counter - WB_GRAPHIC_TEXT_HIGHSCORE1_FRAME_CNT) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT + 1 : WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_CNT;
        glUniform1ui(frame_counter_loc, game->gamestate.frame_counter - WB_GRAPHIC_TEXT_HIGHSCORE1_FRAME_CNT + 3.0 * WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT);
        wbGameDrawText(game, "1. 050000          ", 2.0f, WB_GRAPHIC_TEXT_HIGHSCORE1_FRAME_CNT,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 1,
            replace_color_speed_loc, WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, cnt, WB_GRAPHIC_TEXT_HIGHSCORE_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.text_highscore_animation_colors
        );
        wbGameDrawText(game, "         SENSI SOFT", 1.0f, WB_GRAPHIC_TEXT_HIGHSCORE2_FRAME_CNT,
            0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y + (WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE - WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE) / 2 + 1) / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.gui_red_animation_colors
        );
        glUniform1f(replace_color_reflect_height_loc, 0);
        for (int i = 0; i < 6; i++) {
            int frame = i < 3 ? WB_GRAPHIC_TEXT_SCOREBOARD1_FRAME_CNT : WB_GRAPHIC_TEXT_SCOREBOARD1_FRAME_CNT + WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT;
            cnt = (game->gamestate.frame_counter - frame) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT < WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT ?
                  (game->gamestate.frame_counter - frame) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT + 1 : WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT;
            glUniform1ui(frame_counter_loc, game->gamestate.frame_counter - WB_GRAPHIC_TEXT_SCOREBOARD1_FRAME_CNT + 9.5f * WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT - WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT * i);
            sprintf(text, "   %06i           %01i.               ", 20000, i + 2);
            wbGameDrawText(game, text, 1.0f, frame,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                key_color_mode_loc, 1,
                replace_color_speed_loc, WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED,
                replace_color_cnt_loc, cnt, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
                replace_colors_loc, game->graphic.gui_blue_animation_colors
            );
        }
        glUniform1f(replace_color_reflect_height_loc, WB_GRAPHIC_WINDOW_HEIGHT);
        for (int i = 6; i > 0;) {i--;
            int frame = i >= 3 ? WB_GRAPHIC_TEXT_SCOREBOARD2_FRAME_CNT : WB_GRAPHIC_TEXT_SCOREBOARD2_FRAME_CNT + WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT;
            cnt = (game->gamestate.frame_counter - frame) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT < WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT ?
                  (game->gamestate.frame_counter - frame) * WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED / WB_GRAPHIC_TEXT_COLOR_HEIGHT + 1 : WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT;
            glUniform1ui(frame_counter_loc, game->gamestate.frame_counter - WB_GRAPHIC_TEXT_SCOREBOARD2_FRAME_CNT + WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT * i);
            sprintf(text, "%01i.                     %06i        ", i + 2, 20000);
            wbGameDrawText(game, text, 1.0f, frame,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                key_color_mode_loc, 1,
                replace_color_speed_loc, WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_SPEED,
                replace_color_cnt_loc, cnt, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
                replace_colors_loc, game->graphic.gui_red_animation_colors
            );
        }
        for (int i = 0; i < 6; i++) {
            i == 0 ? sprintf(text, "          %s                 %s %s", "jop", "krx", "nif") : 
                     sprintf(text, "          %s                 %s %s", "---", "---", "---");
            wbGameDrawText(game, text, 1.0f, WB_GRAPHIC_TEXT_SCOREBOARD3_FRAME_CNT,
                0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y + WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y * i) / WB_GRAPHIC_WINDOW_HEIGHT,
                key_color_mode_loc, 0,
                replace_color_speed_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED,
                replace_color_cnt_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
                replace_colors_loc, game->graphic.gui_blue_animation_colors
            );
        }
        wbGameDrawText(game, "one  plr              two  plrs  ", 1.0f, WB_GRAPHIC_TEXT_SCOREBOARD3_FRAME_CNT,
            0.0f, 2.0f - 2.0f * 255 / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_TEXT_TOPSCORES_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_CNT, WB_GRAPHIC_TEXT_SCOREBOARD_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.text_scoreboard_animation_colors
        );
        break;
        
        case WB_GAMESTATE_GETREADY:
        wbGameDrawEntities(game, replace_colors_loc);
        wbGameDrawText(game, "GET READY!", 1.0f, 0,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.gui_blue_animation_colors
        );
        break;

        case WB_GAMESTATE_SPAWN:
        wbGameDrawDust(game, key_alpha_loc);
        wbGameDrawMap(game, &game->map.atlas.background);
        wbGameDrawEntities(game, replace_colors_loc);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWizSpawn(game);
        wbGameDrawMap(game, &game->map.atlas.collider_texture);
        wbGameDrawGui(game, replace_colors_loc);
        break;

        case WB_GAMESTATE_PLAY:
        wbGameDrawDust(game, key_alpha_loc);
        wbGameDrawMap(game, &game->map.atlas.background);
        wbGameDrawEntities(game, replace_colors_loc);
        wbGameDrawPlayerCat(game);
        wbGameDrawPlayerWiz(game);
        wbGameDrawGui(game, replace_colors_loc);
        break;

        case WB_GAMESTATE_DEATH:
        wbGameDrawDust(game, key_alpha_loc);
        wbGameDrawMap(game, &game->map.atlas.background);
        wbGameDrawEntities(game, replace_colors_loc);
        wbGameDrawPlayerCat(game);
        wbGameDrawGui(game, replace_colors_loc);
        break;

        case WB_GAMESTATE_GAMEOVER:
        wbGameDrawDust(game, key_alpha_loc);
        wbGameDrawMap(game, &game->map.atlas.background);
        wbGameDrawGui(game, replace_colors_loc);
        wbGameDrawText(game, "GAME OVER", 1.0f, 0,
            0.0f, 2.0f - 2.0f * (WB_GRAPHIC_TEXT_GAMEOVER_OFFSET_Y) / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.gui_blue_animation_colors
        );
        wbGameDrawText(game, "player one", 1.0f, 0,
            0.0f, 2.0f - 2.0f * WB_GRAPHIC_TEXT_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT,
            key_color_mode_loc, 0,
            replace_color_speed_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED,
            replace_color_cnt_loc, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT, WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT,
            replace_colors_loc, game->graphic.gui_blue_animation_colors
        );
        break;
    }
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
        uint64_t frame_time = (uint64_t)round(glfwGetTime() * WB_FPS);

        // Poll for and process events
        // This checks for input events (keyboard, mouse) and window events (resize, close)
        glfwPollEvents();

        // Run with fps
        if (glfwGetKey(game.window.handle, GLFW_KEY_P)) _sleep(1000 / WB_FPS * 50); /*TODO: for debug*/
        if (!glfwGetKey(game.window.handle, GLFW_KEY_LEFT_BRACKET) && frame_time == game.last_frame_time) continue;

        wbWindowLockAspectRatio(&game.window);

        bool space_pressed = glfwGetKey(game.window.handle, GLFW_KEY_SPACE);

        switch (game.gamestate.state) {
            case WB_GAMESTATE_TITLESCREEN:
            wbGameRender(&game);
            if (space_pressed && !game.window.prev_key_state[GLFW_KEY_SPACE]) {
                wbGamestateSetupGetready(&game.gamestate, &game.sound, &game.map, &game.enemy_buffer, &game.particle_buffer, &game.projectile_buffer);
            }
            game.window.prev_key_state[GLFW_KEY_SPACE] = space_pressed;
            if (game.gamestate.frame_counter > 0 /*WB_GAMEPLAY_TITLESCREEN_FRAME_CNT*/) {}
            break;

            case WB_GAMESTATE_GETREADY:
            uint32_t seed = glfwGetTime() * 1e9;
            if (randfin(seed, 0.0f, 1.0f) < 0.0625f / WB_FPS * 50) {
                wbEnemyInsertRandoms(&game.enemy_buffer, game.gamestate.frame_counter);
            }
            wbGameRender(&game);
            wbEnemyUpdate(&game.enemy_buffer, &game.player.wiz, &game.player.cat, &game.particle_buffer, &game.gamestate, &game.sound);
            if (space_pressed && !game.window.prev_key_state[GLFW_KEY_SPACE]) {
                ma_sound_stop(&game.sound.getready);
                game.gamestate.frame_counter = 0;
                game.gamestate.powerup.slot = -1;
                game.gamestate.powerup.unlocked = game.gamestate.powerup.permanent;
                int pos_x_min = WB_GRAPHIC_MAP_VIEW_WIDTH / 2 - 1;
                int pos_x_max = game.map.atlas.background.width - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + 1;
                wbPlayerWizInit(&game.player.wiz, pos_x_min, pos_x_max);
                wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
                wbPlayerCatInit(&game.player.cat);
                wbBufferClear(&game.enemy_buffer);
                uint32_t seed = time(NULL);
                WBVec2f pos;
                WBVec2f vel = {0.0f};
                for (int i = 0; i < 8; i++) {
                    pos.x = randfin(seed++, 200, 3000);
                    if (pos.x > game.map.view.center_x - WB_GRAPHIC_WINDOW_WIDTH / 2 && pos.x < game.map.view.center_x + WB_GRAPHIC_WINDOW_WIDTH / 2) {
                        i--;
                        continue;
                    }
                    pos.y = randfin(seed++,  50,  200);
                    wbEnemyAppend(&game.enemy_buffer, WB_ENEMY_SPINNERBLUE, &pos, &vel, WB_MOVEPATTERN_INERT);
                }
                game.gamestate.state = WB_GAMESTATE_SPAWN;
            }
            game.window.prev_key_state[GLFW_KEY_SPACE] = space_pressed;
            break;

            case WB_GAMESTATE_SPAWN:
            wbGameProcessInput(&game);
            wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate, game.frame_cnt);
            wbParticleUpdate(&game.particle_buffer, &game.player.wiz, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.player.wiz, &game.player.cat, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            wbGameRender(&game);
            if (game.gamestate.frame_counter * WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED >= WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_FRAME_CNT) {
                game.player.wiz.animation_angle = 0.0f;
                game.gamestate.state = WB_GAMESTATE_PLAY;
            }
            break;

            case WB_GAMESTATE_PLAY:
            if (game.enemy_buffer.head.cnt == 0) {
                ma_sound_seek_to_pcm_frame(&game.sound.clear, 0);
                ma_sound_start(&game.sound.clear);
                WBVec2f pos;
                WBVec2f vel = {0.0f};
                uint32_t seed = time(NULL);
                for (int i = 0; i < 8; i++) { // TODO: temporary
                    pos.x = randfin(seed++, 200, 3000);
                    if (pos.x > game.map.view.center_x - WB_GRAPHIC_WINDOW_WIDTH / 2 && pos.x < game.map.view.center_x + WB_GRAPHIC_WINDOW_WIDTH / 2) {
                        i--;
                        continue;
                    }
                    pos.y = randfin(seed++,   0,  150);
                    wbEnemyAppend(&game.enemy_buffer, WB_ENEMY_CIRCLE, &pos, &vel, WB_MOVEPATTERN_CIRCLE);
                }
            }
            wbGameProcessInput(&game);
            wbPlayerWizHandleCollision(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerWizUpdate(&game.player.wiz, &game.map, &game.gamestate);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate, game.frame_cnt);
            wbParticleUpdate(&game.particle_buffer, &game.player.wiz, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.player.wiz, &game.player.cat, &game.particle_buffer, &game.gamestate, &game.sound);
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
                game.gamestate.frame_counter = 0;
                game.gamestate.state = WB_GAMESTATE_DEATH;
            }
            break;

            case WB_GAMESTATE_DEATH:
            wbGameProcessInput(&game);
            wbPlayerCatUpdate(&game.player.cat, &game.player.wiz, &game.map, &game.gamestate, game.frame_cnt);
            wbParticleUpdate(&game.particle_buffer, &game.player.wiz, &game.gamestate, &game.sound);
            wbEnemyUpdate(&game.enemy_buffer, &game.player.wiz, &game.player.cat, &game.particle_buffer, &game.gamestate, &game.sound);
            wbProjectileUpdate(&game.projectile_buffer, &game.map, &game.player.wiz, &game.enemy_buffer, &game.particle_buffer, &game.gamestate, &game.sound);
            game.gamestate.lifes++;
            wbGameRender(&game);
            game.gamestate.lifes--;
            if (game.gamestate.frame_counter >= WB_GAMERULE_GAMESTATE_HIT_FRAME_CNT) {
                if (game.gamestate.lifes) {
                    wbGamestateSetupGetready(&game.gamestate, &game.sound, &game.map, &game.enemy_buffer, &game.particle_buffer, &game.projectile_buffer);
                } else {
                    ma_sound_seek_to_pcm_frame(&game.sound.gameover, 0);
                    ma_sound_start(&game.sound.gameover);
                    game.gamestate.frame_counter = 0;
                    game.gamestate.state = WB_GAMESTATE_GAMEOVER;
                }
            }
            break;

            case WB_GAMESTATE_GAMEOVER:
            wbGameRender(&game);
            if (game.gamestate.frame_counter >= WB_GAMERULE_GAMESTATE_GAMEOVER_FRAME_CNT) {
                wbGamestateSetupTitlescreen(&game.gamestate, &game.sound);
            }
            break;
        }

        game.gamestate.frame_counter++;
        game.last_frame_time = frame_time;
        game.frame_cnt++;
    }

    wbGameUninit(&game);
    return 0;
}
