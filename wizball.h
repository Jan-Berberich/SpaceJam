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

#define WB_FPS_MAX 9999
#define WB_FPS_MIN 50

#define WB_GAMERULE_PROCESS_INPUT_SPEED 60 /*50*/
#define WB_GAMERULE_PLAYER_WIZ_HANDLE_COLLISION_SPEED 9999 /*50*/

#define WB_MAP_CNT 6
#define WB_POWERUP_SLOT_CNT 7

#define WB_ENEMY_CNT_MAX 64
#define WB_PARTICLE_CNT_MAX 64
#define WB_PROJECTILE_CNT_MAX 64

// GRAPHIC
#define WB_GRAPHIC_VSYNC 1

#define WB_GRAPHIC_SUBPIXEL_CNT 2.0f

// sprite
#define WB_GRAPHIC_SPRITE_SIZE           64.0f
#define WB_GRAPHIC_SPRITE_ATLAS_WIDTH  1024.0f
#define WB_GRAPHIC_SPRITE_ATLAS_HEIGHT 1024.0f
#define WB_GRAPHIC_SPRITE_WIDTH_X  (WB_GRAPHIC_SPRITE_SIZE / WB_GRAPHIC_WINDOW_WIDTH       )
#define WB_GRAPHIC_SPRITE_HEIGHT_Y (WB_GRAPHIC_SPRITE_SIZE / WB_GRAPHIC_WINDOW_HEIGHT      )
#define WB_GRAPHIC_SPRITE_WIDTH_U  (WB_GRAPHIC_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_WIDTH )
#define WB_GRAPHIC_SPRITE_HEIGHT_V (WB_GRAPHIC_SPRITE_SIZE / WB_GRAPHIC_SPRITE_ATLAS_HEIGHT)
// entity
#define WB_GRAPHIC_ENTITY_OFFSET 2.0f * WB_GRAPHIC_MAP_VIEW_OFFSET_Y / WB_GRAPHIC_WINDOW_HEIGHT \
                              + (2.0f * WB_GRAPHIC_MAP_VIEW_HEIGHT - WB_GRAPHIC_SPRITE_SIZE) / WB_GRAPHIC_WINDOW_HEIGHT

// shader
#define WB_GRAPHIC_SPRITE_VERTICES_CNT 4
#define WB_GRAPHIC_SPRITE_INDICES_CNT 6
#define WB_GRAPHIC_BATCH_CNT 64
#define WB_GRAPHIC_RGBA_CNT 4

#define WB_GRAPHIC_KEY_COLOR_R 1.0f
#define WB_GRAPHIC_KEY_COLOR_G 0.0f
#define WB_GRAPHIC_KEY_COLOR_B 1.0f
#define WB_GRAPHIC_KEY_COLOR_A 1.0f
/* #FF00FFFF */

// window
#define WB_GRAPHIC_WINDOW_WIDTH  600.0f /*600.0f*/
#define WB_GRAPHIC_WINDOW_HEIGHT 486.0f /*486.0f*/

// map
#define WB_GRAPHIC_MAP_ATLAS_WIDTH  3365.0f
#define WB_GRAPHIC_MAP_ATLAS_HEIGHT 1920.0f

#define WB_GRAPHIC_MAP_SUBPIXEL_CNT      1.0f /*2.0f*/
#define WB_GRAPHIC_MAP_DUST_SUBPIXEL_CNT 4.0f

#define WB_GRAPHIC_MAP_VIEW_WIDTH   600.0f /*570.0f*/
#define WB_GRAPHIC_MAP_VIEW_HEIGHT (WB_GRAPHIC_MAP_ATLAS_HEIGHT / WB_MAP_CNT)
#define WB_GRAPHIC_MAP_VIEW_OFFSET_Y 70.0f /* 70.0f*/

#define WB_GRAPHIC_MAP_DUST_SPRITE_SIZE 64.0f
#define WB_GRAPHIC_MAP_DUST_LAYER_CNT 4
#define WB_GRAPHIC_MAP_DUST_DENSITY 0.001f
#define WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X 4.0f
#define WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y 2.0f
#define WB_GRAPHIC_MAP_DUST_VELOCITY_FACTOR 0.5f

#define WB_GRAPHIC_MAP_DUST_ATLAS_WIDTH  (WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT)
#define WB_GRAPHIC_MAP_DUST_ATLAS_HEIGHT (WB_GRAPHIC_MAP_DUST_SPRITE_SIZE)
#define WB_GRAPHIC_MAP_DUST_ROW_CNT (int)(WB_GAMERULE_MAP_HORIZON_HEIGHT / (WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_Y) + 1)
#define WB_GRAPHIC_MAP_DUST_COL_CNT (int)(WB_GRAPHIC_MAP_VIEW_WIDTH      / (WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SCALE_X) + 2)

// payer
#define WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_X       (0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPRITE_ATLAS_Y       (0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_X (0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_SPRITE_ATLAS_Y (1.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_X       (6.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PLAYER_CAT_SPRITE_ATLAS_Y       (2.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_0     (0.0f          * 50)
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_1     (1.0f / 21.00f * 50) /* 21    frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_2     (1.0f / 10.67f * 50) /* 32/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_3     (1.0f /  5.33f * 50) /* 16/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_4     (1.0f /  4.27f * 50) /* 47/11 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_5     (1.0f /  3.00f * 50) /*  3    frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_6     (1.0f /  2.67f * 50) /*  8/ 3 frames / animation frame */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_7     (1.0f /  2.00f * 50) /* added for sprint */
#define WB_GRAPHIC_PLAYER_WIZ_ANIMATION_FRAME_CNT 16
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_SPEED (1.0f / 3.0f   * 50)
#define WB_GRAPHIC_PLAYER_WIZ_SPAWN_ANIMATION_FRAME_CNT 22
#define WB_GRAPHIC_PLAYER_CAT_ANIMATION_SPEED       (1.0f / 4.0f   * 50)
#define WB_GRAPHIC_PLAYER_CAT_ANIMATION_FRAME_CNT 3

// enemy
#define WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_X (0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_SPINNERCYAN_SPRITE_ATLAS_Y (6.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_X      (4.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_CIRCLE_SPRITE_ATLAS_Y      (6.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_X     (0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_ENEMY_DROPLET_SPRITE_ATLAS_Y     (7.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_SPEED (1.0f / 6.0f * 50)
#define WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_FRAME_CNT 4
#define WB_GRAPHIC_ENEMY_DROPLET_ANIMATION_SPEED (1.0f / 3.0f * 50)
#define WB_GRAPHIC_ENEMY_DROPLET_ANIMATION_FRAME_CNT 11

#define WB_GRAPHIC_ENEMY_COLORMAP_RED_OFFSET 0
#define WB_GRAPHIC_ENEMY_COLORMAP_GREEN_OFFSET 1
#define WB_GRAPHIC_ENEMY_COLORMAP_BLUE_OFFSET 2
#define WB_GRAPHIC_ENEMY_COLORMAP_CYAN_OFFSET 3
#define WB_GRAPHIC_ENEMY_COLORMAP_OFFSET 4
#define WB_GRAPHIC_ENEMY_COLORMAP_SPEED (1.0f / 5.0f * 50)
#define WB_GRAPHIC_ENEMY_COLORMAP_CNT 4

// particle
#define WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_X       ( 0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_POWERUP_SPRITE_ATLAS_Y       ( 5.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_X         (12.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DECAY_SPRITE_ATLAS_Y         ( 5.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_X  ( 0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DROPLET_FALL_SPRITE_ATLAS_Y  ( 8.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_X ( 7.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_SPRITE_ATLAS_Y ( 8.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PARTICLE_ANIMATION_SPEED (1.0f / 4.0f * 50)
#define WB_GRAPHIC_PARTICLE_ANIMATION_FRAME_CNT 4
#define WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_ANIMATION_CNT 9

// projectile
#define WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_X   ( 0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BULLET_SPRITE_ATLAS_Y   ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_X   ( 1.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLAZER_SPRITE_ATLAS_Y   ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X ( 2.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X  ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y  ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X ( 4.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y ( 3.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLINKER_SPRITE_ATLAS_X  ( 0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BLINKER_SPRITE_ATLAS_Y  ( 4.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_X     (10.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_PROJECTILE_BEAM_SPRITE_ATLAS_Y     ( 2.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_SPEED (1.0f / 4.0f * 50)
#define WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT 3

#define WB_GRAPHIC_PROJECTILE_BEAM_COLORMAP_SPEED (1.0f / 1.0f * 50)
#define WB_GRAPHIC_PROJECTILE_BEAM_COLORMAP_CNT 16 // rkrkgkgkbkbkwkwk

#define WB_GRAPHIC_PROJECTILE_BEAM_OFFSET_Y (48 * 2)

// gui
#define WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_X       ( 0.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_GUI_POWERUP_MAXED_SPRITE_ATLAS_X (13.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_GUI_POWERUP_SPRITE_ATLAS_Y       (14.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_X      (15.0f * WB_GRAPHIC_SPRITE_SIZE)
#define WB_GRAPHIC_GUI_CAULDRON_SPRITE_ATLAS_Y      (14.0f * WB_GRAPHIC_SPRITE_SIZE)

#define WB_GRAPHIC_GUI_LEVEL_COLORMAP_SPEED (1.0f / 2.0f * 50)
#define WB_GRAPHIC_GUI_COLORMAP_SPEED       (1.0f / 4.0f * 50)

#define WB_GRAPHIC_GUI_POWERUP_STRIDE     53.0f
#define WB_GRAPHIC_GUI_CAULDRON_STRIDE    79.0f
#define WB_GRAPHIC_GUI_LOWER_OFFSET_X     (2.5f * WB_GRAPHIC_GUI_CAULDRON_STRIDE)
#define WB_GRAPHIC_GUI_SCORE_OFFSET_Y     63.0f
#define WB_GRAPHIC_GUI_LEVEL_OFFSET_Y      4.0f

#define WB_GRAPHIC_GUI_FPS_LOWPASS_TIMECONST 0.2f

// text
#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_X ( 0.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_ATLAS_Y (60.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_X ( 0.0f * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_ATLAS_Y (61.0f * WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_X ( 0.0f * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_ATLAS_Y (31.0f * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_X 606.0f
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_ATLAS_Y 960.0f

#define WB_GRAPHIC_TEXT_DIGIT_DOT_SPRITE_ATLAS_OFFSET 36
#define WB_GRAPHIC_TEXT_SMALL_MINUS_SPRITE_ATLAS_OFFSET 26
#define WB_GRAPHIC_TEXT_SMALL_DOT_SPRITE_ATLAS_OFFSET 27

#define WB_GRAPHIC_TEXT_SCROLL_COLORMAP_SPEED (2.0f / 3.0f * 50)
#define WB_GRAPHIC_TEXT_MIRROR_COLORMAP_SPEED (1.0f / 2.0f * 50)
#define WB_GRAPHIC_TEXT_BLINK_COLORMAP_SPEED  (1.0f / 4.0f * 50)
#define WB_GRAPHIC_TEXT_PULSE_COLORMAP_SPEED  (1.0f / 7.0f * 50)

#define WB_GRAPHIC_TEXT_CHAR_CNT 32

#define WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE         16.0f
#define WB_GRAPHIC_TEXT_SMALL_SPRITE_SIZE         16.0f
#define WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE         32.0f
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_WIDTH     418.0f
#define WB_GRAPHIC_TEXT_WIZBALL_SPRITE_HEIGHT     32.0f
#define WB_GRAPHIC_TEXT_WIZBALL_OFFSET_Y          86.0f
#define WB_GRAPHIC_TEXT_TOPSCORES_OFFSET_Y       143.0f
#define WB_GRAPHIC_TEXT_HIGHSCORE_OFFEST_Y       199.0f
#define WB_GRAPHIC_TEXT_TOPSCORES_TABLE_OFFSET_Y 287.0f
#define WB_GRAPHIC_TEXT_TOPSCORES_TABLE_STRIDE_Y (2.0f * WB_GRAPHIC_TEXT_DIGIT_SPRITE_SIZE)
#define WB_GRAPHIC_TEXT_GETREADY_OFFSET_Y        (486 - WB_GAMERULE_MAP_HORIZON_HEIGHT - 70 + WB_GRAPHIC_MAP_VIEW_OFFSET_Y)
#define WB_GRAPHIC_TEXT_GAMEOVER_OFFSET_Y        (486 - 224.0f                         - 70 + WB_GRAPHIC_MAP_VIEW_OFFSET_Y)
#define WB_GRAPHIC_TEXT_COLORBAND_HEIGHT 2.0f
#define WB_GRAPHIC_TEXT_SCOREBOARD_DELAY_TIME (  60.0 / 1000)
#define WB_GRAPHIC_TEXT_TOPSCORES_DRAW_TIME   ( 920.0 / 1000)
#define WB_GRAPHIC_TEXT_HIGHSCORE1_DRAW_TIME  (1940.0 / 1000)
#define WB_GRAPHIC_TEXT_HIGHSCORE2_DRAW_TIME  (2260.0 / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD1_DRAW_TIME (3390.0 / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD2_DRAW_TIME (4040.0 / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD3_DRAW_TIME (5240.0 / 1000)
#define WB_GRAPHIC_TEXT_SCOREBOARD4_DRAW_TIME (5260.0 / 1000)

// colormap (max colors per pallet is 32 in shader)
#define WB_GRAPHIC_COLORMAP_ENEMY_0  0x803648FF /* #803648FF */
#define WB_GRAPHIC_COLORMAP_ENEMY_1  0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_ENEMY_2  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_ENEMY_3  0x71BCA9FF /* #71BCA9FF */
#define WB_GRAPHIC_COLORMAP_ENEMY_4  0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_ENEMY_5  0xCCD454FF /* #CCD454FF */
#define WB_GRAPHIC_COLORMAP_ENEMY_6  0x9CE57DFF /* #9CE57DFF */
#define WB_GRAPHIC_COLORMAP_ENEMY_7  0x616DDDFF /* #616DDDFF */
#define WB_GRAPHIC_COLORMAP_ENEMY_CNT 8
#define WB_GRAPHIC_COLORMAP_BEAM_0   0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_BEAM_1   0x803648FF /* #803648FF */
#define WB_GRAPHIC_COLORMAP_BEAM_2   0x9CE57DFF /* #9CE57DFF */
#define WB_GRAPHIC_COLORMAP_BEAM_3   0x616DDDFF /* #616DDDFF */
#define WB_GRAPHIC_COLORMAP_BEAM_4   0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_BEAM_CNT 5
#define WB_GRAPHIC_COLORMAP_BLUE6_0  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_BLUE6_1  0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_BLUE6_2  0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORMAP_BLUE6_3  0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_BLUE6_4  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_BLUE6_5  0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_BLUE6_CNT 6
#define WB_GRAPHIC_COLORMAP_RED8_0   0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORMAP_RED8_1   0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_RED8_2   0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORMAP_RED8_3   0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_RED8_4   0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORMAP_RED8_5   0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_RED8_6   0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORMAP_RED8_7   0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_GREEN8_0 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_GREEN8_1 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORMAP_GREEN8_2 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_GREEN8_3 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_GREEN8_4 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_GREEN8_5 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORMAP_GREEN8_6 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_GREEN8_7 0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_BLUE8_0  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_BLUE8_1  0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_BLUE8_2  0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_BLUE8_3  0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_BLUE8_4  0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_BLUE8_5  0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_BLUE8_6  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_BLUE8_7  0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_RGB8_CNT 8
#define WB_GRAPHIC_COLORMAP_ALL32_0  0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_ALL32_1  0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_ALL32_2  0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_ALL32_3  0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_ALL32_4  0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORMAP_ALL32_5  0x767676FF /* #767676FF */
#define WB_GRAPHIC_COLORMAP_ALL32_6  0xA4A4A4FF /* #A4A4A4FF */
#define WB_GRAPHIC_COLORMAP_ALL32_7  0x767676FF /* #767676FF */
#define WB_GRAPHIC_COLORMAP_ALL32_8  0x4E4E4EFF /* #4E4E4EFF */
#define WB_GRAPHIC_COLORMAP_ALL32_9  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_ALL32_10 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_ALL32_11 0x77BBA2FF /* #77BBA2FF */
#define WB_GRAPHIC_COLORMAP_ALL32_12 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_ALL32_13 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORMAP_ALL32_14 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORMAP_ALL32_15 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_ALL32_16 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORMAP_ALL32_17 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_ALL32_18 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_ALL32_19 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORMAP_ALL32_20 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_COLORMAP_ALL32_21 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORMAP_ALL32_22 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORMAP_ALL32_23 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_ALL32_24 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_COLORMAP_ALL32_25 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_ALL32_26 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORMAP_ALL32_27 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_ALL32_28 0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORMAP_ALL32_29 0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_ALL32_30 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_COLORMAP_ALL32_31 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_ALL32_CNT 32
#define WB_GRAPHIC_COLORMAP_RED4_0   0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_COLORMAP_RED4_1   0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_RED4_2   0x875322FF /* #875322FF */
#define WB_GRAPHIC_COLORMAP_RED4_3   0x5F3D00FF /* #5F3D00FF */
#define WB_GRAPHIC_COLORMAP_GREEN4_0 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_COLORMAP_GREEN4_1 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_GREEN4_2 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_COLORMAP_GREEN4_3 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_COLORMAP_BLUE4_0  0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_COLORMAP_BLUE4_1  0x77BBA2FF /* #77BBA2FF */
#define WB_GRAPHIC_COLORMAP_BLUE4_2  0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_COLORMAP_BLUE4_3  0x000000FF /* #000000FF */
#define WB_GRAPHIC_COLORMAP_RGB4_CNT 4
// ~GRAPHIC

// GAMERULE

// control & movement
#define WB_GAMERULE_AUTOFIRE_HOLD_TIME (12.0f / 50)
#define WB_GAMERULE_POWERUP_WIGGLE_CNT 4
#define WB_GAMERULE_POWERUP_WIGGLE_SPEED (1.0f / 10.0f * 50)
#define WB_GAMERULE_GRAVITY (0.125f * 50 * 50)

// scoress
#define WB_GAMERULE_SCORE_ENEMY 50
#define WB_GAMERULE_SCORE_DROPLET 50
#define WB_GAMERULE_SCORE_POWERUP 100

// map
#define WB_GAMERULE_MAP_CEIL_HEIGHT      4.0f
#define WB_GAMERULE_MAP_FLOOR_HEIGHT   288.0f
#define WB_GAMERULE_MAP_HORIZON_HEIGHT 240.0f
#define WB_GAMERULE_VIEW_BULLET_WIZ_CNT_MAX (int)round(2.0f * WB_GRAPHIC_MAP_VIEW_WIDTH / 570)
#define WB_GAMERULE_VIEW_BULLET_CAT_CNT_MAX (int)round(1.0f * WB_GRAPHIC_MAP_VIEW_WIDTH / 570)

// player wiz
#define WB_GAMERULE_PLAYER_WIZ_HEALTH_MAX 1

#define WB_GAMERULE_PLAYER_WIZ_SPAWN_POS_Y 244.25f
#define WB_GAMERULE_PLAYER_WIZ_SPAWN_VEL_Y (-7.5f * 50)

#define WB_GAMERULE_PLAYER_WIZ_ACC_X (0.5f   * 50)
#define WB_GAMERULE_PLAYER_WIZ_DEC_X (0.125f * 50)
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_0 (0.0f        * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_1 (1.0f / 8.0f * 50 * 2) /*   1 px / 8 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_2 (1.0f / 4.0f * 50 * 2) /*   1 px / 4 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_3 (1.0f / 2.0f * 50 * 2) /*   1 px / 2 frames */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_4 (1.0f / 1.0f * 50 * 2) /*   1 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_5 (1.5f / 1.0f * 50 * 2) /* 1.5 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_6 (2.0f / 1.0f * 50 * 2) /*   2 px / 1 frame  */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_7 (3.0f / 1.0f * 50 * 2) /* (added for sprint) */
#define WB_GAMERULE_PLAYER_WIZ_VEL_X_CNT 8
#define WB_GAMERULE_PLAYER_WIZ_ACC_Y (0.33333333333333333 * 50)
#define WB_GAMERULE_PLAYER_WIZ_DEC_Y (0.33333333333333333 * 50)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_0 (0.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_1 (1.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_2 (2.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_3 (3.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_4 (4.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_WIZ_VEL_Y_CNT 5

#define WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS 16 /* w: 37, h: 38 */
#define WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT 32
#define WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN sinf(M_2PI / 16.0f)

// player cat
#define WB_GAMERULE_PLAYER_CAT_HEALTH_MAX 9

#define WB_GAMERULE_PLAYER_CAT_WIDTH        24.0f
#define WB_GAMERULE_PLAYER_CAT_CEIL_OFFSET   6.0f
#define WB_GAMERULE_PLAYER_CAT_FLOOR_OFFSET 22.0f

#define WB_GAMERULE_PLAYER_CAT_VEL (4.0f * 50 * 2)
#define WB_GAMERULE_PLAYER_CAT_MOVEDELAY_INPUT_FRAME_CNT (8 * WB_GAMERULE_PROCESS_INPUT_SPEED / 50)

#define WB_GAMERULE_PLAYER_CAT_REST_OFFSET_VEL (WB_GAMERULE_PLAYER_CAT_VEL / 2)
#define WB_GAMERULE_PLAYER_CAT_REST_OFFSET_X 40.0f

// entity
#define WB_GAMERULE_ENEMY_HITBOX_SIZE 48.0f // ?
#define WB_GAMERULE_PARTICLE_DROPLET_FALL_VEL (1.0f * 50 * 2)
#define WB_GAMERULE_PROJECTILE_VEL            (8.0f * 50 * 2)
#define WB_GAMERULE_PROJECTILE_BEAM_HITBOX_SIZE (48.0f * 2)
#define WB_GAMERULE_PARTICLE_HITBOX_SIZE 48.0f // ?
#define WB_GAMERULE_PARTICLE_POWERUP_DROP_CHANCE 0.1f // ?

// gamestate
#define WB_GAMERULE_GAMESTATE_HIT_TIME 4.0
#define WB_GAMERULE_GAMESTATE_GAMEOVER_TIME 6.0
#define WB_GAMERULE_POWERUP_SHIELD_WEAROFF_TIME 36.0 /* dec by 10 every hit */
#define WB_GAMERULE_POWERUP_SHIELD_WARN_TIME 33.0 /* TODO: play sound */

// Paths
#define WB_MAP_BACKGROUND_ATLAS_PATH "sprite/map_background_atlas.png"
#define WB_MAP_FOREGROUND_ATLAS_PATH "sprite/map_foreground_atlas.png"
#define WB_SPRITE_ATLAS_PATH         "sprite/sprite_atlas.png"

// Sounds
#define WB_SOUND_TITLESCREEN_PATH      "sound/titlescreen.wav"
#define WB_SOUND_GETREADY_PATH         "sound/getready.wav"
#define WB_SOUND_FIRE_PATH             "sound/fire.wav"
#define WB_SOUND_POWERUP_DROP_PATH     "sound/powerup_drop.wav"
#define WB_SOUND_POWERUP_COLLECT_PATH  "sound/powerup_collect.wav"
#define WB_SOUND_POWERUP_ACTIVATE_PATH "sound/powerup_activate.wav"
#define WB_SOUND_DECAY_PATH            "sound/decay.wav"
#define WB_SOUND_CLEAR_PATH            "sound/clear.wav"
#define WB_SOUND_BLINKER_PATH          "sound/blinker.wav"
#define WB_SOUND_BOMB_PATH             "sound/bomb.wav"
#define WB_SOUND_CATHIT_PATH           "sound/cathit.wav"
#define WB_SOUND_WIZDEATH_PATH         "sound/wizdeath.wav"
#define WB_SOUND_GAMEOVER_PATH         "sound/gameover.wav"

// Key Binding
#define WB_KEY_Ü         GLFW_KEY_LEFT_BRACKET
#define WB_KEY_MINUS     GLFW_KEY_SLASH
#define WB_KEY_PLUS      GLFW_KEY_RIGHT_BRACKET
#define WB_KEY_CONFIRM_0 GLFW_KEY_ENTER
#define WB_KEY_CONFIRM_1 GLFW_KEY_SPACE
#define WB_KEY_BACK      GLFW_KEY_BACKSPACE
#define WB_KEY_ESCAPE    GLFW_KEY_ESCAPE
#define WB_KEY_WIZ_LEFT  GLFW_KEY_A
#define WB_KEY_WIZ_RIGHT GLFW_KEY_D
#define WB_KEY_WIZ_UP    GLFW_KEY_W
#define WB_KEY_WIZ_DOWN  GLFW_KEY_S
#define WB_KEY_WIZ_FIRE  GLFW_KEY_SPACE
#define WB_KEY_CAT_LEFT  GLFW_KEY_LEFT
#define WB_KEY_CAT_RIGHT GLFW_KEY_RIGHT
#define WB_KEY_CAT_UP    GLFW_KEY_UP
#define WB_KEY_CAT_DOWN  GLFW_KEY_DOWN
#define WB_KEY_CAT_FIRE  GLFW_KEY_RIGHT_CONTROL
/*from here not in original game*/
#define WB_KEY_POWERUP            GLFW_KEY_F
#define WB_KEY_POWERUP_TOGGLEGRAV GLFW_KEY_LEFT_CONTROL
#define WB_KEY_POWERUP_LEFT       WB_KEY_MINUS
#define WB_KEY_POWERUP_RIGHT      WB_KEY_PLUS
#define WB_KEY_SPRINT             GLFW_KEY_LEFT_SHIFT
#define WB_KEY_SLOWDOWN           GLFW_KEY_P
#define WB_KEY_SPEEDUP            WB_KEY_Ü
#define WB_KEY_ALTL               GLFW_KEY_LEFT_ALT
#define WB_KEY_ALTR               GLFW_KEY_RIGHT_ALT
#define WB_KEY_ALT_FULLSCREEN     GLFW_KEY_ENTER
#define WB_KEY_ALT_VSYNC          GLFW_KEY_V
#define WB_KEY_ALT_FPS            GLFW_KEY_F
#define WB_MOUSE_CAT_MOVE         GLFW_MOUSE_BUTTON_RIGHT
#define WB_MOUSE_CAT_FIRE         GLFW_MOUSE_BUTTON_LEFT

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
    WB_ENEMY_DROPLET,
    WB_ENEMY_SPINNERYELLOW,
    WB_ENEMY_SPINNERCYAN,
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
    WB_PARTICLE_DROPLET_FALL,
    WB_PARTICLE_DROPLET_SPLAT,
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
    WB_DIRECTION_NEUTRAL = 0,
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
    WB_COLORMODE_CYCLE,
    WB_COLORMODE_SCROLL,
    WB_COLORMODE_FILL
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
    bool retreat;
    WBDirectionType facing;
    WBDirectionType next_spray_direction;
    float rest_offset_x;
    float pos_y_buffer[WB_GAMERULE_PLAYER_CAT_MOVEDELAY_INPUT_FRAME_CNT];
    int pos_y_buffer_idx;
} WBCat;

typedef struct {
    WBWiz wiz;
    WBCat cat;
} WBPlayer;

typedef struct {
    WBEntityHead head;
    WBMovepatternType movepattern_type;
    double time;
    WBVec2f vel;
    int attack_period;
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
    GLint time;
    GLint key_color;
    GLint key_alpha;
    GLint tex_size;
    GLint key_color_mode;
    GLint window_scale;
    GLint replace_color_mirror_height;
    GLint fill_level;
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
    int pos_x, pos_y, width, height;
} WBViewport;

typedef struct {
    GLFWwindow* handle;
    int width;
    int height;
    WBViewport viewport;
    int windowed_x, windowed_y, windowed_width, windowed_height;
    bool is_fullscreen;
    bool vsync;
} WBWindow;

typedef struct {
    ma_engine _engine;
    ma_sound titlescreen;
    ma_sound getready;
    ma_sound fire;
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
    uint32_t beam[WB_GRAPHIC_COLORMAP_BEAM_CNT];
    uint32_t enemy[WB_GRAPHIC_COLORMAP_ENEMY_CNT];
    uint32_t red4[WB_GRAPHIC_COLORMAP_RGB4_CNT];
    uint32_t green4[WB_GRAPHIC_COLORMAP_RGB4_CNT];
    uint32_t blue4[WB_GRAPHIC_COLORMAP_RGB4_CNT];
    uint32_t red8[WB_GRAPHIC_COLORMAP_RGB8_CNT];
    uint32_t green8[WB_GRAPHIC_COLORMAP_RGB8_CNT];
    uint32_t blue8[WB_GRAPHIC_COLORMAP_RGB8_CNT];
    uint32_t blue6[WB_GRAPHIC_COLORMAP_BLUE6_CNT];
    uint32_t all32[WB_GRAPHIC_COLORMAP_ALL32_CNT];
} WBColormap;

typedef struct {
    GLuint sprite_atlas_texture_id;
    GLuint background_atlas_texture_id;
    GLuint foreground_atlas_texture_id;
    GLuint dust_atlas_texture_id;
    WBColormap colormap;
    char text[WB_GRAPHIC_TEXT_CHAR_CNT];
} WBGraphic;

typedef struct {
    int bullet_wiz_cnt;
    int bullet_cat_cnt;
    bool beam;
    bool spray;
    float center_x;
} WBView;

typedef struct {
    bool* collider_atlas;
    WBView view;
} WBMap;

typedef struct {
    WBGamestateType state;
    WBPowerup powerup;
    int cauldron_levels[3];
    int score, lifes, enemy_cnt;
    int highscore;
    int score2;
    int level;
    double time;
    double delta_time;
} WBGamestate;

typedef struct {
    WBGamestate gamestate;
    WBWindow window;
    WBMap map;
    WBSound sound;
    WBShader shader;
    WBGraphic graphic;
    WBPlayer player;
    WBEnemyBuffer enemy_buffer;
    WBParticleBuffer particle_buffer;
    WBProjectileBuffer projectile_buffer;
} WBGame;

extern bool wbWindowInit(WBWindow* window);
extern void wbWindowLockAspectRatio(WBWindow* window);
extern void wbWindowToggleFullscreen(WBWindow* window);

extern void wbPlayerWizInit(WBWiz* wiz, float pos_x_min, float pos_x_max);
extern void wbPlayerWizHandleCollision(WBWiz* wiz, WBMap* map, WBGamestate* gamestate);
extern void wbPlayerWizUpdate(WBWiz* wiz, WBMap* map, WBGamestate* gamestate);

extern void wbPlayerCatInit(WBCat* cat);
extern void wbPlayerCatUpdate(WBCat* cat, WBWiz* wiz, WBMap* map, WBGamestate* gamestate);

extern int wbBufferAppend(void* buffer, uint8_t object_type, WBVec2f* pos);
extern void wbBufferRemove(void* buffer, int idx);
extern void wbBufferClear(void* buffer);

extern void wbEnemyPopulate(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_tpye, int colormap_offset, WBMovepatternType movepattern_type, WBView* view);
extern void wbEnemyInsertRandoms(WBEnemyBuffer* enemy_buffer, double time);
extern void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBMap* map, WBPlayer* player, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);
extern void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbParticleUpdate(WBParticleBuffer* particle_buffer, WBPlayer* player, WBGamestate* gamestate, WBSound* sound);

extern void wbProjectileBufferInit(WBProjectileBuffer* projectile_buffer);
extern int wbProjectileAppend(WBProjectileBuffer* projectile_buffer, WBProjectileType type, WBVec2f* pos, WBVec2f* vel);
extern void wbProjectileUpdate(WBProjectileBuffer* projectile_buffer, WBMap* map, WBWiz* wiz, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbShaderInit(WBShader* shader);
extern void wbShaderUninit(WBShader* shader);

extern bool wbMapInitCollider(WBMap* map, uint8_t* data);
extern bool wbMapGetCollision(WBMap* map, int x, int y, int level);

extern bool wbGraphicInit(WBGraphic* graphic, WBMap* map);
extern void wbGraphicUninit(WBGraphic* graphic, WBMap* map);

extern bool wbSoundInit(WBSound* sound);
extern void wbSoundUninit(WBSound* sound);

extern void wbGamestateSetupTitlescreen(WBGamestate* gamestate, WBSound* sound);
extern void wbGamestateSetupGetready(WBGamestate* gamestate, WBSound* sound, WBView* view, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBProjectileBuffer* projectile_buffer);

extern int wbGameRun();

#endif // WB_H
