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
//#define WB_NO_SOUND
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
#include <windows.h>

#include "utils.h"



#define WB_FPS 1000 /*50*/
#define WB_GAMEPLAY_PROCESS_INPUT_FRAME_CNT (WB_FPS / 50) /*1*/

#define WB_WINDOW_WIDTH  600 /*600*/
#define WB_WINDOW_HEIGHT 486 /*486*/

#define WB_SUBPIXEL_CNT 2.0f
#define WB_MAP_SUBPIXEL_CNT 1.0f /*2.0f*/
#define WB_MAP_DUST_SUBPIXEL_CNT (2 * WB_MAP_SUBPIXEL_CNT)

#define WB_MAP_VIEW_WIDTH 600 /*570*/
#define WB_MAP_VIEW_OFFSET_Y 70 /*70*/
#define WB_MAP_CEIL_HEIGHT 4
#define WB_MAP_FLOOR_HEIGHT 288
#define WB_MAP_HORIZON_HEIGHT 240

#define WB_MAP_CNT 6

#define WB_MAP_DUST_SPRITE_SIZE 64
#define WB_MAP_DUST_LAYER_CNT 4
#define WB_MAP_DUST_DENSITY 0.001f
#define WB_MAP_DUST_SPRITE_SCALE_X 4
#define WB_MAP_DUST_SPRITE_SCALE_Y 2
#define WB_MAP_DUST_VELOCITY_FACTOR 0.5f

#define WB_WINDOW_POWERUP_STRIDE 53
#define WB_WINDOW_SCORE_OFFSET_Y 63
#define WB_WINDOW_LEVEL_OFFSET_X 108
#define WB_WINDOW_LEVEL_OFFSET_Y 4

#define WB_SCORE_ENEMY 50
#define WB_SCORE_DROPLET 50
#define WB_SCORE_POWERUP 100

#define WB_GAMEPLAY_HIT_FRAME_CNT (3 * WB_FPS)
#define WB_GAMEPLAY_GAMEOVER_FRAME_CNT (5 * WB_FPS)

#define WB_PLAYER_WIZ_HEALTH_MAX 1
#define WB_PLAYER_CAT_HEALTH_MAX 9

#define WB_MAP_VIEW_BULLET_WIZ_CNT_MAX (int)round(2.0f * WB_MAP_VIEW_WIDTH / 570)
#define WB_MAP_VIEW_BULLET_CAT_CNT_MAX (int)round(1.0f * WB_MAP_VIEW_WIDTH / 570)

#define WB_ENEMY_HITBOX_SIZE 40 // ?
#define WB_PROJECTILE_BEAM_HITBOX_SIZE (48 * 2)
#define WB_PARTICLE_HITBOX_SIZE 48 // ?
#define WB_PARTICLE_POWERUP_DROP_CHANCE 0.1f // ?

#define WB_PLAYER_WIZ_WIDTH 37
#define WB_PLAYER_WIZ_HEIGHT 38
#define WB_PLAYER_WIZ_COLLISION_RADIUS 16
#define WB_PLAYER_WIZ_COLLISION_ANGLE_CNT 32
#define WB_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN sinf(M_2PI / 16.0f)

#define WB_PLAYER_WIZ_INIT_POS_Y (WB_PLAYER_WIZ_HEIGHT / 2 + WB_MAP_CEIL_HEIGHT)

#define WB_PLAYER_CAT_WIDTH 24
#define WB_PLAYER_CAT_CEIL_OFFSET 6
#define WB_PLAYER_CAT_FLOOR_OFFSET 22

// movement
// 1 px / 8 frames, 21    frames / animation frame 21.00
// 1 px / 4 frames, 32/ 3 frames / animation frame 10.67
// 1 px / 2 frames, 16/ 3 frames / animation frame  5.33
// 1 px / 1 frame,  47/11 frames / animation frame  4.27
//1.5px / 1 frame,   3    frames / animation frame  3.00
// 2 px / 1 frame,   8/ 3 frames / animation frame  2.67
#define WB_PLAYER_WIZ_GRAVITY (0.125f * 50 * 50 / WB_FPS / WB_FPS)

#define WB_PLAYER_WIZ_ACC_X (0.5f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_DEC_X (0.125f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_0 (0.0f        * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_1 (1.0f / 8.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_2 (1.0f / 4.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_3 (1.0f / 2.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_4 (1.0f / 1.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_5 (1.5f / 1.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_6 (2.0f / 1.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_7 (3.0f / 1.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_X_CNT 8
#define WB_PLAYER_WIZ_ANIMATION_SPEED_0 (0.0f          * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_1 (1.0f / 21.00f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_2 (1.0f / 10.67f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_3 (1.0f /  5.33f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_4 (1.0f /  4.27f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_5 (1.0f /  3.00f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_6 (1.0f /  2.67f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_7 (1.0f /  2.00f * 50 / WB_FPS)

#define WB_PLAYER_WIZ_ACC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_PLAYER_WIZ_DEC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_0 (0.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_Y_1 (1.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_Y_2 (2.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_Y_3 (3.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_Y_4 (4.0f * 50 / WB_FPS * 2)
#define WB_PLAYER_WIZ_VEL_Y_CNT 5

#define WB_PLAYER_CAT_VEL (4.0f * 50 / WB_FPS * 2)
// ~movement

#define WB_ENEMY_ANIMATION_COLOR_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_ENEMY_ANIMATION_COLOR_CNT 4
#define WB_ENEMY_ANIMATION_COLOR_0 0xFFFFFFFF /* #FFFFFFFF */
#define WB_ENEMY_ANIMATION_COLOR_1 0xCCD454FF /* #CCD454FF */
#define WB_ENEMY_ANIMATION_COLOR_2 0x9CE57DFF /* #9CE57DFF */
#define WB_ENEMY_ANIMATION_COLOR_3 0x616DDDFF /* #616DDDFF */

#define WB_PROJECTILE_ANIMATION_COLOR_CNT 5
#define WB_PROJECTILE_ANIMATION_COLOR_0 0x000000FF /* #000000FF */
#define WB_PROJECTILE_ANIMATION_COLOR_1 0x803648FF /* #803648FF */
#define WB_PROJECTILE_ANIMATION_COLOR_2 0x9CE57DFF /* #9CE57DFF */
#define WB_PROJECTILE_ANIMATION_COLOR_3 0x616DDDFF /* #616DDDFF */
#define WB_PROJECTILE_ANIMATION_COLOR_4 0xFFFFFFFF /* #FFFFFFFF */

#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_CNT 6
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_0 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_1 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_2 0x000000FF /* #000000FF */
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_3 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_4 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_5 0xA4A4A4FF /* #A4A4A4FF */

#define WB_GRAPHIC_GUI_LEVEL_ANIMATION_COLOR_SPEED (1.0f / 2.0f * 50 / WB_FPS)
#define WB_GRAPHIC_GUI_ANIMATION_COLOR_SPEED (1.0f / 4.0f * 50 / WB_FPS)
#define WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT 8
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_0 0x000000FF /* #000000FF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_1 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_2 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_3 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_4 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_5 0xAF6882FF /* #AF6882FF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_6 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_GUI_RED_ANIMATION_COLOR_7 0x7E3650FF /* #7E3650FF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_0 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_1 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_2 0x000000FF /* #000000FF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_3 0x55A33EFF /* #55A33EFF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_4 0xCED54CFF /* #CED54CFF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_5 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_6 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_GUI_GREEN_ANIMATION_COLOR_7 0x99E681FF /* #99E681FF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_0 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_1 0x000000FF /* #000000FF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_2 0x312BB5FF /* #312BB5FF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_3 0x8439ACFF /* #8439ACFF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_4 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_5 0xFFFFFFFF /* #FFFFFFFF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_6 0x6A65EEFF /* #6A65EEFF */
#define WB_GRAPHIC_GUI_BLUE_ANIMATION_COLOR_7 0x8439ACFF /* #8439ACFF */

#define WB_POWERUP_WIGGLE_CNT 4
#define WB_POWERUP_WIGGLE_SPEED (1.0f / 10.0f * 50 / WB_FPS)

#define WB_POWERUP_SLOT_CNT 7

#define WB_SPRITE_SIZE 64
#define WB_LETTER_SPRITE_SIZE 32
#define WB_DIGIT_SPRITE_SIZE 16

#define WB_ENEMY_SPINNERBLUE_ANIMATION_SPEED (1.0f / 6.0f * 50 / WB_FPS)
#define WB_ENEMY_CIRCLE_ANIMATION_SPEED (1.0f / 5.0f * 50 / WB_FPS)

#define WB_PLAYER_WIZ_AUTOFIRE_FRAME_CNT (12 * WB_FPS / 50)

#define WB_PLAYER_WIZ_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_PLAYER_WIZ_SPRITE_ATLAS_Y (0 * WB_SPRITE_SIZE)
#define WB_PLAYER_CAT_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_PLAYER_CAT_SPRITE_ATLAS_Y (1 * WB_SPRITE_SIZE)

#define WB_PLAYER_CAT_REST_OFFSET_X 40
#define WB_PLAYER_CAT_REST_OFFSET_VEL (WB_PLAYER_CAT_VEL / 2)

#define WB_PLAYER_CAT_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)

#define WB_PLAYER_WIZ_ANIMATION_FRAME_CNT 16
#define WB_PLAYER_CAT_ANIMATION_FRAME_CNT 3

#define WB_PLAYER_CAT_MOVEDELAY_FRAME_CNT (8 * WB_FPS / 50)

#define WB_ENEMY_CNT_MAX 64
#define WB_PARTICLE_CNT_MAX 64
#define WB_PROJECTILE_CNT_MAX 64

#define WB_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT 4
#define WB_ENEMY_CIRCLE_ANIMATION_FRAME_CNT 1

#define WB_PARTICLE_POWERUP_ANIMATION_FRAME_CNT 1

#define WB_PARTICLE_DECAY_ANIMATION_FRAME_CNT 4
#define WB_PARTICLE_DECAY_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)

#define WB_PROJECTILE_BEAM_SPRITE_ATLAS_X (10 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BEAM_SPRITE_ATLAS_Y ( 1 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BEAM_ANIMATION_COLOR_CNT 16 // rkrkgkgkbkbkwkwk
#define WB_PROJECTILE_BEAM_ANIMATION_COLOR_SPEED (1.0f / 1.0f * 50 / WB_FPS)
#define WB_PROJECTILE_BEAM_ANIMATION_FRAME_CNT 3
#define WB_PROJECTILE_BEAM_ANIMATION_SPEED (1.0f / 4.0f * 50 / WB_FPS)

#define WB_PROJECTILE_VEL (8.0f * 50 / WB_FPS * 2)
#define WB_PROJECTILE_BULLET_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BULLET_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BLAZER_SPRITE_ATLAS_X (1 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BLAZER_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X (3 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X (4 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BLINKER_SPRITE_ATLAS_X (5 * WB_SPRITE_SIZE)
#define WB_PROJECTILE_BLINKER_SPRITE_ATLAS_Y (2 * WB_SPRITE_SIZE)

#define WB_PARTICLE_POWERUP_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_PARTICLE_POWERUP_SPRITE_ATLAS_Y (4 * WB_SPRITE_SIZE)
#define WB_PARTICLE_DECAY_SPRITE_ATLAS_X (12 * WB_SPRITE_SIZE)
#define WB_PARTICLE_DECAY_SPRITE_ATLAS_Y ( 4 * WB_SPRITE_SIZE)

#define WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_ENEMY_SPINNERBLUE_SPRITE_ATLAS_Y (5 * WB_SPRITE_SIZE)
#define WB_ENEMY_CIRCLE_SPRITE_ATLAS_X (4 * WB_SPRITE_SIZE)
#define WB_ENEMY_CIRCLE_SPRITE_ATLAS_Y (5 * WB_SPRITE_SIZE)

#define WB_POWERUP_SPRITE_ATLAS_X ( 0 * WB_SPRITE_SIZE)
#define WB_POWERUP_MAXED_SPRITE_ATLAS_X (15 * WB_SPRITE_SIZE)
#define WB_POWERUP_SPRITE_ATLAS_Y (14 * WB_SPRITE_SIZE)

#define WB_LETTER_SPRITE_ATLAS_X (0 * WB_LETTER_SPRITE_SIZE)
#define WB_LETTER_SPRITE_ATLAS_Y (31 * WB_LETTER_SPRITE_SIZE)
#define WB_DIGIT_SPRITE_ATLAS_X (54 * WB_DIGIT_SPRITE_SIZE)
#define WB_DIGIT_SPRITE_ATLAS_Y (63 * WB_DIGIT_SPRITE_SIZE)

// Paths
#define WB_MAP_BACKGROUND_ATLAS_PATH "sprite/map_background_atlas.png"
#define WB_MAP_COLLIDER_ATLAS_PATH "sprite/map_collider_atlas.png"
#define WB_SPRITE_ATLAS_PATH "sprite/sprite_atlas.png"
#define WB_SOUND_FIRE_PATH "sound/fire.wav"
#define WB_SOUND_FIRE_SPAM_PATH "sound/fire_spam.wav"
#define WB_SOUND_POWERUP_DROP_PATH "sound/powerup_drop.wav"
#define WB_SOUND_POWERUP_COLLECT_PATH "sound/powerup_collect.wav"
#define WB_SOUND_POWERUP_ACTIVATE_PATH "sound/powerup_activate.wav"
#define WB_SOUND_DECAY_PATH "sound/decay.wav"

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
    WB_GAMESTATE_HIT,
    WB_GAMESTATE_PAUSE,
    WB_GAMESTATE_GAMEOVER,
    WB_GAMESTATE_SCOREBOARD,
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
    WB_MOVEPATTERN_JAY,
    WB_MOVEPATTERN_RAID
} WBMovepatternType;

typedef enum {
    WB_DIRECTION_NEGATIVE = -1,
    WB_DIRECTION_POSITIVE = 1,
} WBDirectionType;

typedef enum {
    WB_BUFFER_ENEMY,
    WB_BUFFER_PARTICLE,
    WB_BUFFER_PROJECTILE,
    WB_BUFFER_CNT
} WBBufferType;

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
    uint32_t animation_colors[WB_GRAPHIC_GUI_POWERUP_ANIMATION_COLOR_CNT];
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
} WBEntityHead;

typedef struct {
    WBVec2f pos, vel;
    int health;
    float vel_x_values[WB_PLAYER_WIZ_VEL_X_CNT];
    float animation_speed_values[WB_PLAYER_WIZ_VEL_X_CNT];
    float vel_y_values[WB_PLAYER_WIZ_VEL_Y_CNT];
    float animation_angle;
    float vel_x_key, vel_y_key;
    float collider_angles[WB_PLAYER_WIZ_COLLISION_ANGLE_CNT];
    WBVec2f collision_vec;
    WBDirectionType facing;
    WBDirectionType next_bullet_direction;
    WBDirectionType next_spray_direction;
} WBWiz;

typedef struct {
    WBVec2f pos;
    WBVec2f vel;
    bool hold_position;
    int health;
    WBDirectionType next_spray_direction;
    WBDirectionType facing;
    float rest_offset_x;
    float pos_y_buffer[WB_PLAYER_CAT_MOVEDELAY_FRAME_CNT];
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
    uint64_t frame_age;
} WBEnemy;

typedef struct {
    WBEntityHead head;
    float frame_age;
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
    uint32_t animation_colors[WB_ENEMY_ANIMATION_COLOR_CNT];
} WBEnemyBuffer;

typedef struct {
    WBBufferHead head;
    WBParticle entries[WB_PARTICLE_CNT_MAX];
} WBParticleBuffer;

typedef struct {
    WBBufferHead head;
    WBProjectile entries[WB_PROJECTILE_CNT_MAX];
    uint32_t animation_colors[WB_PROJECTILE_ANIMATION_COLOR_CNT];
} WBProjectileBuffer;

typedef struct {
    int bullet_wiz_cnt;
    int bullet_cat_cnt;
    bool beam;
    bool spray;
    float center_x;
} WBView;

typedef struct {
    WBTexture dust;
    WBTexture background;
    WBTexture collider_texture;
    bool* collider;
} WBMapAtlas;

typedef struct {
    WBMapAtlas atlas;
    WBView view;
} WBMap;

typedef struct {
    float vertices[4 * 4];
    unsigned int indices[3 * 2];
    GLuint vbo, vao, ebo, program;
} WBShader;

typedef struct {
    GLFWwindow* handle;
    int width;
    int height;
} WBWindow;

typedef struct {
    ma_engine engine;
    ma_sound fire;
    ma_sound fire_spam;
    ma_sound powerup_drop;
    ma_sound powerup_collect;
    ma_sound powerup_activate;
    ma_sound decay;
} WBSound;

typedef struct {
    WBGamestateType state;
    WBPowerup powerup;
    int score, lifes, enemy_cnt;
    int highscore;
    int score2;
    int level;
    uint32_t red_animation_colors[WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT];
    uint32_t green_animation_colors[WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT];
    uint32_t blue_animation_colors[WB_GRAPHIC_GUI_ANIMATION_COLOR_CNT];
} WBGamestate;

typedef struct {
    WBGamestate gamestate;
    WBWindow window;
    WBShader shader;
    WBTexture sprite_atlas;
    WBMap map;
    WBSound sound;
    double last_frame_time;
    uint64_t frame_cnt;
    uint64_t frame_counter;
    WBPlayer player;
    WBEnemyBuffer enemy_buffer;
    WBParticleBuffer particle_buffer;
    WBProjectileBuffer projectile_buffer;
    char text[WB_WINDOW_WIDTH / WB_DIGIT_SPRITE_SIZE];
} WBGame;

extern bool wbWindowInit(WBWindow* window);
extern void wbWindowLockAspectRatio(WBWindow* window);

extern void wbPlayerWizInit(WBWiz* wiz, float pos_x_min, float pos_x_max);
extern void wbPlayerWizHandleCollision(WBWiz* wiz, WBMap* map, WBGamestate* gamestate);
extern void wbPlayerWizUpdate(WBWiz* wiz, WBMap* map, WBGamestate* gamestate);

extern void wbPlayerCatInit(WBCat* cat);
extern void wbPlayerCatUpdate(WBCat* cat, WBWiz* wiz, WBMap* map, WBGamestate* gamestate, uint64_t frame_cnt);

extern void* wbBufferAppend(void* buffer, uint8_t object_type, WBVec2f* pos);
extern void wbBufferRemove(void* buffer, int idx);
extern void wbBufferClear(void* buffer);

extern void wbEnemyAppend(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_type, WBVec2f* pos, WBVec2f* vel, WBMovepatternType movepattern_type);
extern void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBWiz* wiz, WBCat* cat, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);
extern void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbParticleUpdate(WBParticleBuffer* particle_buffer, WBWiz* wiz, WBGamestate* gamestate, WBSound* sound);

extern void wbProjectileBufferInit(WBProjectileBuffer* projectile_buffer);
extern void wbProjectileAppend(WBProjectileBuffer* projectile_buffer, WBProjectileType type, WBVec2f* pos, WBVec2f* vel);
extern void wbProjectileUpdate(WBProjectileBuffer* projectile_buffer, WBMap* map, WBWiz* wiz, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound);

extern void wbShaderInit(WBShader* shader);

extern bool wbMapInit(WBMap* map);
extern bool wbMapGetCollision(WBMap* map, int x, int y, int level);

extern void wbTextureInit(WBTexture* texture, uint8_t* data, int width, int height);

extern int wbGameRun();

#endif // WB_H
