#ifndef WB_H
#define WB_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_SIMD
#define STBI_NO_HDR
#define STBI_ONLY_PNG
#include "stb_image.h"

// for faster build
#define WB_NO_SOUND
#ifdef WB_NO_SOUND
#define MA_SUCCESS 0
#define ma_engine_init(_1,_2) MA_SUCCESS
#define ma_sound_init_from_file(_1,_2,_3,_4,_5,_6) MA_SUCCESS
#define ma_sound_seek_to_pcm_frame(_1,_2) MA_SUCCESS
#define ma_sound_start(_) MA_SUCCESS
#define ma_sound_stop(_) MA_SUCCESS
#define ma_sound_uninit(_) MA_SUCCESS
#define ma_engine_uninit(_) MA_SUCCESS
#define ma_engine void*
#define ma_sound void*
#else
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_NO_ENCODING
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_GENERATION
#include "miniaudio.h"
#endif

#define M_2PI (2.0 * 3.141592653589793)
#include <math.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "utils.h"



#define WB_FPS 250 /*50*/

#define WB_MAP_CNT 6
#define WB_POWERUP_SLOT_CNT 7

#define WB_ENEMY_CNT_MAX 64
#define WB_PARTICLE_CNT_MAX 64
#define WB_PROJECTILE_CNT_MAX 64

// GRAPHIC
#define WB_GRAPHIC_SUBPIXEL_CNT 2.0f

#define WB_GRAPHIC_SPRITE_SIZE 64
#define WB_GRAPHIC_SPRITE_VERTICES_CNT 4
#define WB_GRAPHIC_SPRITE_INDICES_CNT 6
#define WB_GRAPHIC_BATCH_CNT 64

#define WB_GRAPHIC_KEY_COLOR_R 1.0f
#define WB_GRAPHIC_KEY_COLOR_G 0.0f
#define WB_GRAPHIC_KEY_COLOR_B 1.0f
#define WB_GRAPHIC_KEY_COLOR_A 1.0f
/* #FF00FFFF */

// window
#define WB_GRAPHIC_WINDOW_WIDTH  600 /*600*/
#define WB_GRAPHIC_WINDOW_HEIGHT 486 /*486*/

// map
#define WB_GRAPHIC_MAP_SUBPIXEL_CNT 1.0f /*2.0f*/
#define WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT 4.0f

#define WB_GRAPHIC_VIEW_WIDTH 600 /*570*/
#define WB_GRAPHIC_VIEW_OFFSET_Y 70 /*70*/

#define WB_GRAPHIC_MAP_DUST_SPRITE_SIZE 64
#define WB_GRAPHIC_MAP_DUST_LAYER_CNT 4
#define WB_GRAPHIC_MAP_DUST_DENSITY 0.001f
#define WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X 4
#define WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y 2
#define WB_GRAPHIC_MAP_DUST_VELOCITY_FACTOR 0.5f

// payer
#define WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_Y (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_Y (1 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_X (6 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_Y (2 * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_0 (0.0f          * 50 / WB_FPS)
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_1 (1.0f / 21.00f * 50 / WB_FPS) /* 21    frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_2 (1.0f / 10.67f * 50 / WB_FPS) /* 32/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_3 (1.0f /  5.33f * 50 / WB_FPS) /* 16/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_4 (1.0f /  4.27f * 50 / WB_FPS) /* 47/11 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_5 (1.0f /  3.00f * 50 / WB_FPS) /*  3    frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_6 (1.0f /  2.67f * 50 / WB_FPS) /*  8/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_7 (1.0f /  2.00f * 50 / WB_FPS) /* added for sprint */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_FRAME_CNT 16
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED (1.0f / 3.0f * 50 / WB_FPS)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_FRAME_CNT 22
#define WB_GRAPHIC_PLAYER_CAT_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_PLAYER_CAT_ANIMATION_FRAME_CNT 3

// enemy
#define WB_GRAPHIC_ENEMY_SPINNERBLUE_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_SPINNERBLUE_SPRITE_ATLAS_Y (6 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_X (4 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_Y (6 * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_ENEMY_SPINNERBLUE_ANIMATION_SPEED (1.0f / 6.0f * 50 / WB_FPS)
#define WB_GRAPHIC_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT 4

#define WB_GRAPHIC_ENEMY_COLORPALLET_SPEED (1.0f / 5.0f * 50 / WB_FPS)
#define WB_GRAPHIC_ENEMY_COLORPALLET_CNT 4
#define WB_GRAPHIC_ENEMY_COLORPALLET_OFFSET 1

// particle
#define WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_Y (5 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_X (12 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_Y ( 5 * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PARTICLE_DECAY_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_PARTICLE_DECAY_ANIMATION_FRAME_CNT 4

// projectile
#define WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X (1 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X (2 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X (4 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y (3 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLINKER_SPRITE_ATLAS_X (0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLINKER_SPRITE_ATLAS_Y (4 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X (10 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y ( 2 * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT 3

#define WB_GRAPHIC_PROJECTILE_BEAM_COLORPALLET_SPEED (1.0f / 1.0f * 50 / WB_FPS)
#define WB_GRAPHIC_PROJECTILE_BEAM_COLORPALLET_CNT 16 // rkrkgkgkbkbkwkwk

#define WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y (48 * 2)

// gui
#define WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X ( 0 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X (15 * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_Y (14 * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_GUI_LEVEL_COLORPALLET_SPEED (1.0f / 2.0f * 50 / WB_FPS)
#define WB_GRAPHIC_GUI_COLORPALLET_SPEED (1.0f / 4.0f * 50 / WB_FPS)

#define WB_GRAPHIC_GUI_POWERUP_STRIDE 53
#define WB_GRAPHIC_GUI_SCORE_OFFSET_Y 63
#define WB_GRAPHIC_GUI_LEVEL_OFFSET_Y 4

// text
#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X (0 * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y (60 * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_X (0 * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_Y (61 * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_X (0 * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_Y (31 * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_X 606
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_Y 960

#define WB_GRAPHIC_TEXT_DIGIT_DOT_SPRITE_ATLAS_OFFSET 36
#define WB_GRAPHIC_TEXT_SMALL_MINUS_SPRITE_ATLAS_OFFSET 26
#define WB_GRAPHIC_TEXT_SMALL_DOT_SPRITE_ATLAS_OFFSET 27

#define WB_GRAPHIC_TEXT_SCROLL_COLORPALLET_SPEED (2.0f / 3.0f * 50 / WB_FPS)
#define WB_GRAPHIC_TEXT_MIRROR_COLORPALLET_SPEED (1.0f / 2.0f * 50 / WB_FPS)
#define WB_GRAPHIC_TEXT_BLINK_COLORPALLET_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_TEXT_PULSE_COLORPALLET_SPEED (1.0f / 7.0f * 50 / WB_FPS)

#define WB_GRAPHIC_TEXT_CHAR_CNT 32

#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE 16
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE 16
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE 32
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH 418
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT 32
#define WB_GRAPHIC_TEXT_OFFSET_Y 240
#define WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y 86
#define WB_GRAPHIC_TEXT_TOPSCORES_OFFSET_Y 143
#define WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y 199
#define WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y 287
#define WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y (2 * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_GAMEOVER_OFFSET_Y (WB_GRAPHIC_TEXT_OFFSET_Y - 11)
#define WB_GRAPHIC_TEXT_COLORBAND_HEIGHT 2
#define WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_FRAME_CNT (60.0 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_TOPSCORES_DRAW_FRAME (920 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_HIGHSCORE1_DRAW_FRAME (1940 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_HIGHSCORE2_DRAW_FRAME (2260 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD1_DRAW_FRAME (3390 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD2_DRAW_FRAME (4040 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD3_DRAW_FRAME (5240 * WB_FPS / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD4_DRAW_FRAME (5260 * WB_FPS / 1000)

// colorpallet (max colors per pallet is 32 in shader)
#define WB_GRAPHIC_COLORPALLET_ENEMY_0 0xFF0000FF
#define WB_GRAPHIC_COLORPALLET_ENEMY_1 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_ENEMY_2 0xCCD454FF /* #CCD454FF */
#define WB_GRAPHIC_COLORPALLET_ENEMY_3 0x9CE57DFF /* #9CE57DFF */
#define WB_GRAPHIC_COLORPALLET_ENEMY_4 0x616DDDFF /* #616DDDFF */
#define WB_GRAPHIC_COLORPALLET_ENEMY_CNT 5
#define WB_GRAPHIC_COLORPALLET_BEAM_0 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_BEAM_1 0x803648FF /* #803648FF */
#define WB_GRAPHIC_COLORPALLET_BEAM_2 0x9CE57DFF /* #9CE57DFF */
#define WB_GRAPHIC_COLORPALLET_BEAM_3 0x616DDDFF /* #616DDDFF */
#define WB_GRAPHIC_COLORPALLET_BEAM_4 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_BEAM_CNT 5
#define WB_GRAPHIC_COLORPALLET_BLUE6_0 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_1 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_2 0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_3 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_4 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_5 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_BLUE6_CNT 6
#define WB_GRAPHIC_COLORPALLET_RED8_0 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORPALLET_RED8_1 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_RED8_2 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORPALLET_RED8_3 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_RED8_4 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORPALLET_RED8_5 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_RED8_6 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORPALLET_RED8_7 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_0 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_1 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_2 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_3 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_4 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_5 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_6 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORPALLET_GREEN8_7 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_0 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_1 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_2 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_3 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_4 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_5 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_6 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_BLUE8_7 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_RGB8_CNT 8
#define WB_GRAPHIC_COLORPALLET_ALL32_0  0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_1  0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_2  0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_3  0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_4  0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_5  0x767676FF /* #767676FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_6  0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_7  0x767676FF /* #767676FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_8  0x4E4E4EFF /* #4E4E4EFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_9  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_10 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_11 0x77BBA2FF /* #77BBA2FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_12 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_13 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_14 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_15 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_16 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_17 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_18 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_19 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_20 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_21 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_22 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_23 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_24 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_25 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_26 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_27 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_28 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_29 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_30 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORPALLET_ALL32_31 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_ALL32_CNT 32
#define WB_GRAPHIC_COLORPALLET_RED4_0 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORPALLET_RED4_1 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_RED4_2 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORPALLET_RED4_3 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORPALLET_GREEN4_0 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORPALLET_GREEN4_1 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_GREEN4_2 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORPALLET_GREEN4_3 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORPALLET_BLUE4_0 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORPALLET_BLUE4_1 0x77BBA2FF /* #77BBA2FF */
#define WB_GRAPHIC_COLORPALLET_BLUE4_2 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORPALLET_BLUE4_3 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORPALLET_RGB4_CNT 4
// ~GRAPHIC

// GAMERULE

// control & movement
#define WB_GAMERULE_PROCESS_INPUT_FRAME_CNT (WB_FPS / 50)
#define WB_GAMERULE_AUTOFIRE_FRAME_CNT (12 * WB_FPS / 50)
#define WB_GAMERULE_POWERUP_WIGGLE_CNT 4
#define WB_GAMERULE_POWERUP_WIGGLE_SPEED (1.0f / 10.0f * 50 / WB_FPS)
#define WB_GAMERULE_GRAVITY (0.125f * 50 * 50 / WB_FPS / WB_FPS)

// scoress
#define WB_GAMERULE_SCORE_ENEMY 50
#define WB_GAMERULE_SCORE_DROPLET 50
#define WB_GAMERULE_SCORE_POWERUP 100

// map
#define WB_GAMERULE_MAP_CEIL_HEIGHT 4
#define WB_GAMERULE_MAP_FLOOR_HEIGHT 288
#define WB_GAMERULE_MAP_HORIZON_HEIGHT 240
#define WB_GAMERULE_VIEW_BULLET_WIZ_CNT_MAX (int)round(2.0f * WB_GRAPHIC_VIEW_WIDTH / 570)
#define WB_GAMERULE_VIEW_BULLET_CAT_CNT_MAX (int)round(1.0f * WB_GRAPHIC_VIEW_WIDTH / 570)

// player wiz
#define WB_GAMERULE_PLAYER_WIZ_HEALTH_MAX 1

#define WB_GAMERULE_PLAYER_WIZ_SPAWN_POS_Y 244.25
#define WB_GAMERULE_PLAYER_WIZ_SPAWN_VEL_Y (-7.5 * 50 / WB_FPS)

#define WB_GAMERULE_PLAYER_WIZ_ACC_X (0.5f * 50 / WB_FPS)
#define WB_GAMERULE_PLAYER_WIZ_DEC_X (0.125f * 50 / WB_FPS)
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_0 (0.0f        * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_1 (1.0f / 8.0f * 50 / WB_FPS * 2) /*   1 px / 8 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_2 (1.0f / 4.0f * 50 / WB_FPS * 2) /*   1 px / 4 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_3 (1.0f / 2.0f * 50 / WB_FPS * 2) /*   1 px / 2 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_4 (1.0f / 1.0f * 50 / WB_FPS * 2) /*   1 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_5 (1.5f / 1.0f * 50 / WB_FPS * 2) /* 1.5 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_6 (2.0f / 1.0f * 50 / WB_FPS * 2) /*   2 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_7 (3.0f / 1.0f * 50 / WB_FPS * 2) /* (added for sprint) */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT 8
#define WB_GAMERULE_PLAYER_WIZ_ACC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_GAMERULE_PLAYER_WIZ_DEC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_0 (0.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_1 (1.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_2 (2.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_3 (3.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_4 (4.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT 5

#define WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS 16 /* w: 37, h: 38 */
#define WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT 32
#define WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN sinf(M_2PI / 16.0f)

// player cat
#define WB_GAMERULE_PLAYER_CAT_HEALTH_MAX 9

#define WB_GAMERULE_PLAYER_CAT_REST_OFFSET_VEL (WB_GAMERULE_PLAYER_CAT_VEL / 2)
#define WB_GAMERULE_PLAYER_CAT_REST_OFFSET_X 40

#define WB_GAMERULE_PLAYER_CAT_WIDTH 24
#define WB_GAMERULE_PLAYER_CAT_CEIL_OFFSET 6
#define WB_GAMERULE_PLAYER_CAT_FLOOR_OFFSET 22

#define WB_GAMERULE_PLAYER_CAT_VEL (4.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PLAYER_CAT_MOVEDELAY_FRAME_CNT (8 * WB_FPS / 50)

// entity
#define WB_GAMERULE_ENEMY_HITBOX_SIZE 48 // ?
#define WB_GAMERULE_PROJECTILE_VEL (8.0f * 50 / WB_FPS * 2)
#define WB_GAMERULE_PROJECTILE_BEAM_HITBOX_SIZE (48 * 2)
#define WB_GAMERULE_PARTICLE_HITBOX_SIZE 48 // ?
#define WB_GAMERULE_PARTICLE_POWERUP_DROP_CHANCE 0.1f // ?

// gamestate
#define WB_GAMERULE_GAMESTATE_HIT_FRAME_CNT (4 * WB_FPS)
#define WB_GAMERULE_GAMESTATE_GAMEOVER_FRAME_CNT (6 * WB_FPS)
#define WB_GAMERULE_POWERUP_SHIELD_WEAROFF_TIME 36.0f /* dec by 10 every hit */
#define WB_GAMERULE_POWERUP_SHIELD_WARN_TIME 33.0f /* TODO: play sound */

// Paths
#define WB_MAP_BACKGROUND_ATLAS_PATH "sprite/map_background_atlas.png"
#define WB_MAP_FOREGROUND_ATLAS_PATH "sprite/map_foreground_atlas.png"
#define WB_SPRITE_ATLAS_PATH "sprite/sprite_atlas.png"

// Sounds
#define WB_SOUND_TITLESCREEN_PATH "sound/titlescreen.wav"
#define WB_SOUND_GETREADY_PATH "sound/getready.wav"
#define WB_SOUND_FIRE_PATH "sound/fire.wav"
#define WB_SOUND_FIRE_SPAM_PATH "sound/fire_spam.wav"
#define WB_SOUND_POWERUP_DROP_PATH "sound/powerup_drop.wav"
#define WB_SOUND_POWERUP_COLLECT_PATH "sound/powerup_collect.wav"
#define WB_SOUND_POWERUP_ACTIVATE_PATH "sound/powerup_activate.wav"
#define WB_SOUND_DECAY_PATH "sound/decay.wav"
#define WB_SOUND_CLEAR_PATH "sound/clear.wav"
#define WB_SOUND_BLINKER_PATH "sound/blinker.wav"
#define WB_SOUND_BOMB_PATH "sound/bomb.wav"
#define WB_SOUND_CATHIT_PATH "sound/cathit.wav"
#define WB_SOUND_WIZDEATH_PATH "sound/wizdeath.wav"
#define WB_SOUND_GAMEOVER_PATH "sound/gameover.wav"

// Key Bindings
#define WB_KEY_WIZ_LEFT GLFW_KEY_A
#define WB_KEY_WIZ_RIGHT GLFW_KEY_D
#define WB_KEY_WIZ_UP GLFW_KEY_W
#define WB_KEY_WIZ_DOWN GLFW_KEY_S
#define WB_KEY_WIZ_FIRE GLFW_KEY_SPACE
#define WB_KEY_CAT_LEFT GLFW_KEY_LEFT
#define WB_KEY_CAT_RIGHT GLFW_KEY_RIGHT
#define WB_KEY_CAT_UP GLFW_KEY_UP
#define WB_KEY_CAT_DOWN GLFW_KEY_DOWN
#define WB_KEY_CAT_FIRE GLFW_KEY_RIGHT_CONTROL
#define WB_KEY_POWERUP GLFW_KEY_F /*not in real game*/
#define WB_KEY_TOGGLEGRAV GLFW_KEY_LEFT_CONTROL /*not in ral game*/
#define WB_KEY_SPRINT GLFW_KEY_LEFT_SHIFT /*not in real game*/

// Enums

typedef enum {
    WB_GAMESTATE_TITLESCREEN,
    WB_GAMESTATE_PLAYERSELECT,
    WB_GAMESTATE_GETREADY,
    WB_GAMESTATE_SPAWN,
    WB_GAMESTATE_PLAY,
    WB_GAMESTATE_DEATH,
    WB_GAMESTATE_PAUSE,
    WB_GAMESTATE_GAMEOVER,
    WB_GAMESTATE_HIGHSCORE,
    WB_GAMESTATE_WIZTIPS,
    WB_GAMESTATE_CNT
} WBGamestateType;

typedef enum {
    WB_ENEMY_NONE,
    WB_ENEMY_BLOB,
    WB_ENEMY_SPINNERYELLOW,
    WB_ENEMY_SPINNERBLUE,
    WB_ENEMY_CIRCLE,
    WB_ENEMY_SQUARE,
    WB_ENEMY_RHOMBUS,
    WB_ENEMY_GLIDER,
    WB_ENEMY_JUMPER,
    WB_ENEMY_STOMPER,
    WB_ENEMY_BLINKER,
    WB_ENEMY_CNT
} WBEnemyType;

typedef enum {
    WB_PARTICLE_NONE,
    WB_PARTICLE_POWERUP,
    WB_PARTICLE_DECAY,
    WB_PARTICLE_DROPLET,
    WB_PARTICLE_CNT
} WBParticleType;

typedef enum {
    WB_PROJECTILE_NONE,
    WB_PROJECTILE_BULLET_WIZ,
    WB_PROJECTILE_BULLET_CAT,
    WB_PROJECTILE_BLAZER_WIZ,
    WB_PROJECTILE_BLAZER_CAT,
    WB_PROJECTILE_SPRAY_NW,
    WB_PROJECTILE_SPRAY_N,
    WB_PROJECTILE_SPRAY_NE,
    WB_PROJECTILE_BEAM,
    WB_PROJECTILE_CNT
} WBProjectileType;

typedef enum {
    WB_MOVEPATTERN_INERT,
    WB_MOVEPATTERN_GLIDE,
    WB_MOVEPATTERN_STEP_DOWN,
    WB_MOVEPATTERN_STEP_UP,
    WB_MOVEPATTERN_CIRCLE,
    WB_MOVEPATTERN_BOUNCE,
    WB_MOVEPATTERN_BOUNCE_FLOOR,
    WB_MOVEPATTERN_BOUNCE_CEIL,
    WB_MOVEPATTERN_ARC,
    WB_MOVEPATTERN_RAID,
    WB_MOVEPATTERN_LENSE
} WBMovepatternType;

typedef enum {
    WB_DIRECTION_NEGATIVE = -1,
    WB_DIRECTION_POSITIVE = 1
} WBDirectionType;

typedef enum {
    WB_BUFFER_ENEMY,
    WB_BUFFER_PARTICLE,
    WB_BUFFER_PROJECTILE,
    WB_BUFFER_CNT
} WBBufferType;

typedef enum {
    WB_TEXT_DIGIT,
    WB_TEXT_SMALL,
    WB_TEXT_LARGE,
    WB_TEXT_TITLE
} WBTextType;

typedef enum {
    WB_COLORMODE_FILL,
    WB_COLORMODE_SCROLL
} WBColorMode;

typedef enum {
    WB_POWERUP_NONE     = 0b0000000000000000,
    WB_POWERUP_SLOTMASK = 0b0000000000000011,

    WB_POWERUP_THRUST   = 0b0000000000000001,
    WB_POWERUP_ANTIGRAV = 0b0000000000000010,

    WB_POWERUP_BEAM     = 0b0000000000000100,
    WB_POWERUP_DOUBLE   = 0b0000000000001000,

    //################# = 0b0000000000010000,
    WB_POWERUP_CAT      = 0b0000000000100000,

    //################# = 0b0000000001000000,
    WB_POWERUP_BLAZERS  = 0b0000000010000000,

    WB_POWERUP_WIZSPRAY = 0b0000000100000000,
    WB_POWERUP_CATSPRAY = 0b0000001000000000,

    //################# = 0b0000010000000000,
    WB_POWERUP_BOMB     = 0b0000100000000000,

    //################# = 0b0001000000000000,
    WB_POWERUP_SHIELD   = 0b0010000000000000,
} WBPowerupType;

// Structs

typedef struct {
    float x, y;
} WBVec2f;

typedef struct {
    WBPowerupType unlocked;
    WBPowerupType permanent;
    int slot;
} WBPowerup;

typedef struct {
    int width;
    int height;
    GLuint texture_id;
} WBTexture;

typedef struct {
    uint8_t type;
    WBVec2f pos;
    float color_key;
    float animation_key;
} WBEntityHead;

typedef struct {
    int health;
    WBVec2f pos, vel;
    float vel_x_values[WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT];
    float animation_speed_values[WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT];
    float vel_y_values[WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT];
    float animation_angle;
    float vel_x_key, vel_y_key;
    float collider_angles[WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT];
    WBVec2f collision_vec;
    WBDirectionType facing;
    WBDirectionType next_bullet_direction;
    WBDirectionType next_spray_direction;
} WBWiz;

typedef struct {
    int health;
    WBVec2f pos, vel;
    bool hold_position;
    WBDirectionType facing;
    WBDirectionType next_spray_direction;
    float rest_offset_x;
    float pos_y_buffer[WB_GAMERULE_PLAYER_CAT_MOVEDELAY_FRAME_CNT];
} WBCat;

typedef struct {
    WBWiz wiz;
    WBCat cat;
} WBPlayer;

typedef struct {
    WBEntityHead head;
    WBMovepatternType movepattern_type;
    WBVec2f vel;
    int attack_period;
    float frame_age;
} WBEnemy;

typedef struct {
    WBEntityHead head;
} WBParticle;

typedef struct {
    WBEntityHead head;
    WBVec2f vel;
} WBProjectile;

typedef struct {
    WBBufferType type;
    int cnt;
} WBBufferHead;

typedef struct {
    WBBufferHead head;
    WBEnemy entries[WB_ENEMY_CNT_MAX];
} WBEnemyBuffer;

typedef struct {
    WBBufferHead head;
    WBParticle entries[WB_PARTICLE_CNT_MAX];
} WBParticleBuffer;

typedef struct {
    WBBufferHead head;
    WBProjectile entries[WB_PROJECTILE_CNT_MAX];
} WBProjectileBuffer;

typedef struct {
    int bullet_wiz_cnt;
    int bullet_cat_cnt;
    bool beam;
    bool spray;
    float center_x;
} WBView;

typedef struct {
    WBTexture dust_texture;
    WBTexture background_atlas;
    WBTexture foreground_atlas;
    bool* collider;
} WBMap;

typedef struct {
    GLint key_color;
    GLint frame_counter;
    GLint delta_time;
    GLint key_alpha;
    GLint tex_size;
    GLint key_color_mode;
    GLint window_scale;
    GLint replace_color_mirror_height;
    GLint replace_colorband_height;
    GLint replace_color_speed;
    GLint replace_color_cnt;
    GLint replace_colors;
    GLint subpixel_cnt;
} WBUniformLoc;

typedef struct {
    float vertices[WB_GRAPHIC_SPRITE_VERTICES_CNT * 4 * WB_GRAPHIC_BATCH_CNT]; // x, y, u, v
    unsigned int indices[WB_GRAPHIC_SPRITE_INDICES_CNT * WB_GRAPHIC_BATCH_CNT]; // 2 tris
    int batch_cnt;
    GLuint vbo, vao, ebo, program;
    WBUniformLoc loc;
} WBShader;

typedef struct {
    GLFWwindow* handle;
    int width;
    int height;
    int prev_key_state[GLFW_KEY_LAST + 1];
} WBWindow;

typedef struct {
    ma_engine engine;
    ma_sound titlescreen;
    ma_sound getready;
    ma_sound fire;
    ma_sound fire_spam;
    ma_sound powerup_drop;
    ma_sound powerup_collect;
    ma_sound powerup_activate;
    ma_sound decay;
    ma_sound clear;
    ma_sound blinker;
    ma_sound bomb;
    ma_sound cathit;
    ma_sound wizdeath;
    ma_sound gameover;
} WBSound;

typedef  struct {
    uint32_t beam[WB_GRAPHIC_COLORPALLET_BEAM_CNT];
    uint32_t enemy[WB_GRAPHIC_COLORPALLET_ENEMY_CNT];
    uint32_t red4[WB_GRAPHIC_COLORPALLET_RGB4_CNT];
    uint32_t green4[WB_GRAPHIC_COLORPALLET_RGB4_CNT];
    uint32_t blue4[WB_GRAPHIC_COLORPALLET_RGB4_CNT];
    uint32_t red8[WB_GRAPHIC_COLORPALLET_RGB8_CNT];
    uint32_t green8[WB_GRAPHIC_COLORPALLET_RGB8_CNT];
    uint32_t blue8[WB_GRAPHIC_COLORPALLET_RGB8_CNT];
    uint32_t blue6[WB_GRAPHIC_COLORPALLET_BLUE6_CNT];
    uint32_t all32[WB_GRAPHIC_COLORPALLET_ALL32_CNT];
} WBColorpallet;

typedef struct {
    WBTexture sprite_atlas;
    WBMap map;
    WBColorpallet colorpallet;
    char text[WB_GRAPHIC_TEXT_CHAR_CNT];
} WBGraphic;

typedef struct {
    WBGamestateType state;
    WBPowerup powerup;
    int score, lifes, enemy_cnt;
    int highscore;
    int score2;
    int level;
    uint64_t frame_counter;
} WBGamestate;

typedef struct {
    WBGamestate gamestate;
    WBWindow window;
    WBShader shader;
    WBView view;
    WBSound sound;
    WBGraphic graphic;
    double last_frame_time;
    uint64_t frame_cnt;
    WBPlayer player;
    WBEnemyBuffer enemy_buffer;
    WBParticleBuffer particle_buffer;
    WBProjectileBuffer projectile_buffer;
} WBGame;

extern bool wbWindowInit(WBWindow* window);
extern void wbWindowLockAspectRatio(WBWindow* window);

extern void wbPlayerWizInit(WBWiz* wiz, float pos_x_min, float pos_x_max);
extern void wbPlayerWizHandleCollision(WBWiz* wiz, WBMap* map, WBGamestate* gamestate);
extern void wbPlayerWizUpdate(WBWiz* wiz, WBMap* map, WBView* view, WBGamestate* gamestate);

extern void wbPlayerCatInit(WBCat* cat);
extern void wbPlayerCatUpdate(WBCat* cat, WBWiz* wiz, WBMap* map, WBView* view, WBGamestate* gamestate, uint64_t frame_cnt);

extern int wbBufferAppend(void* buffer, uint8_t object_type, WBVec2f* pos);
extern void wbBufferRemove(void* buffer, int idx);
extern void wbBufferClear(void* buffer);

extern int wbEnemyAppend(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_type, WBVec2f* pos, WBVec2f* vel, WBMovepatternType movepattern_type);
extern void wbEnemyInsertRandoms(WBEnemyBuffer* enemy_buffer, uint64_t frame_counter);
extern void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBWiz* wiz, WBCat* cat, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);
extern void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbParticleUpdate(WBParticleBuffer* particle_buffer, WBWiz* wiz, WBGamestate* gamestate, WBSound* sound);

extern void wbProjectileBufferInit(WBProjectileBuffer* projectile_buffer);
extern int wbProjectileAppend(WBProjectileBuffer* projectile_buffer, WBProjectileType type, WBVec2f* pos, WBVec2f* vel);
extern void wbProjectileUpdate(WBProjectileBuffer* projectile_buffer, WBMap* map, WBView* view, WBWiz* wiz, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbShaderInit(WBShader* shader);

extern bool wbMapInit(WBMap* map);
extern bool wbMapGetCollision(WBMap* map, int x, int y, int level);

extern void wbTextureInit(WBTexture* texture, uint8_t* data, int width, int height);

extern void wbGamestateSetupTitlescreen(WBGamestate* gamestate, WBSound* sound);
extern void wbGamestateSetupGetready(WBGamestate* gamestate, WBSound* sound, WBView* view, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBProjectileBuffer* projectile_buffer);

extern int wbGameRun();

#endif // WB_H
