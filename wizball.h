#ifndef WB_H
#define WB_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "stb_image.h"

#define M_2PI (2.0 * 3.141592653589793)
#include <math.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"



#define WB_FPS 1000 /*50*/

#define WB_WINDOW_WIDTH  640 /*600*/
#define WB_WINDOW_HEIGHT 339 /*486*/

#define WB_SUBPIXEL_X_CNT 0.5 /*2*/
#define WB_SUBPIXEL_Y_CNT 2
#define WB_VELOCITY_FACTOR 2

#define WB_MAP_VIEW_WIDTH 640 /*570*/
#define WB_MAP_VIEW_OFFSET_Y 0 /*70*/
#define WB_MAP_CEIL_HEIGHT 4
#define WB_MAP_FLOOR_HEIGHT 288

#define WB_PLAYER_WIZ_HEALTH_MAX 1
#define WB_PLAYER_CAT_HEALTH_MAX 9

#define WB_PLAYER_WIZ_ONSCREEN_BULLET_CNT_MAX 2
#define WB_PLAYER_CAT_ONSCREEN_BULLET_CNT_MAX 1

#define WB_HITBOX_SIZE 8 // ?

#define WB_PLAYER_WIZ_WIDTH 37
#define WB_PLAYER_WIZ_HEIGHT 38
#define WB_PLAYER_WIZ_COLLISION_RADIUS 16 /*(WB_PLAYER_WIZ_WIDTH / 2)*/
#define WB_PLAYER_WIZ_COLLISION_ANGLE_CNT 32
#define WB_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN sinf(M_2PI / 16.0f)

#define WB_PLAYER_WIZ_INIT_POS_Y (WB_PLAYER_WIZ_HEIGHT / 2 + WB_MAP_CEIL_HEIGHT)

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
#define WB_PLAYER_WIZ_VEL_X_0 (0.0f        * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_1 (1.0f / 8.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_2 (1.0f / 4.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_3 (1.0f / 2.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_4 (1.0f / 1.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_5 (1.5f / 1.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_6 (2.0f / 1.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_X_CNT 7
#define WB_PLAYER_WIZ_ANIMATION_SPEED_0 (0.0f          * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_1 (1.0f / 21.00f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_2 (1.0f / 10.67f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_3 (1.0f /  5.33f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_4 (1.0f /  4.27f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_5 (1.0f /  3.00f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_ANIMATION_SPEED_6 (1.0f /  2.67f * 50 / WB_FPS)

#define WB_PLAYER_WIZ_ACC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_PLAYER_WIZ_DEC_Y (0.33333333333333333 * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_0 (0.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_1 (1.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_2 (2.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_3 (3.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_4 (4.0f * 50 / WB_FPS)
#define WB_PLAYER_WIZ_VEL_Y_CNT 5
// ~movement

#define WB_SPRITE_SIZE 64

#define WB_PLAYER_WIZ_SPRITE_ATLAS_X (0 * WB_SPRITE_SIZE)
#define WB_PLAYER_WIZ_SPRITE_ATLAS_Y (0 * WB_SPRITE_SIZE)
#define WB_PLAYER_CAT_SPRITE_ATLAS_X (1 * WB_SPRITE_SIZE)
#define WB_PLAYER_CAT_SPRITE_ATLAS_Y (0 * WB_SPRITE_SIZE)

#define WB_PLAYER_WIZ_ANIMATION_FRAME_CNT 16
#define WB_PLAYER_CAT_SPRITE_ATLAS_FRAME_CNT 3

#define WB_PLAYER_CAT_MOVEDELAY_FRAME_CNT 8

#define WB_ENEMY_CNT_MAX 64
#define WB_PARTICLE_CNT_MAX 64

// Key Bindings
#define WB_KEY_WIZ_LEFT GLFW_KEY_A
#define WB_KEY_WIZ_RIGHT GLFW_KEY_D
#define WB_KEY_WIZ_UP GLFW_KEY_W
#define WB_KEY_WIZ_DOWN GLFW_KEY_S
#define WB_KEY_WIZ_SHOOT GLFW_KEY_SPACE
#define WB_KEY_CAT_LEFT GLFW_KEY_LEFT
#define WB_KEY_CAT_RIGHT GLFW_KEY_RIGHT
#define WB_KEY_CAT_UP GLFW_KEY_UP
#define WB_KEY_CAT_DOWN GLFW_KEY_DOWN
#define WB_KEY_CAT_SHOOT GLFW_KEY_RIGHT_CONTROL

// Enums

typedef enum {
    WB_GAMESTATE_TITLESCREEN,
    WB_GAMESTATE_PLAYERSELECT,
    WB_GAMESTATE_GETREADY,
    WB_GAMESTATE_PLAYING,
    WB_GAMESTATE_PAUSED,
    WB_GAMESTATE_GAMEOVER,
    WB_GAMESTATE_SCOREBOARD,
    WB_GAMESTATE_CNT
} WBGamestateType;

typedef enum {
    WB_SPRITE_PLAYER,
    WB_SPRITE_ENEMY,
    WB_SPRITE_PARTICLE,
    WB_SPRITE_CNT
} WBSpriteType;

typedef enum {
    WB_ENEMY_BLOB,
    WB_ENEMY_SPINNERYELLOW,
    WB_ENEMY_SPINNERBLUE,
    WB_ENEMY_CIRCLE,
    WB_ENEMY_SQUARE,
    WB_ENEMY_RHOMBUS,
    WB_ENEMY_GLIDER,
    WB_ENEMY_JUMPER,
    WB_ENEMY_STOMPER,
    WB_ENEMY_CNT
} WBEnemyType;

typedef enum {
    WB_PARTICLE_DROP,
    WB_PARTICLE_PROJECTILE,
    WB_PARTICLE_BEAM,
    WB_PARTICLE_POWERUP,
    WB_PARTICLE_DECAY,
    WB_PARTICLE_BLINKER,
    WB_PARTICLE_CNT
} WBParticleType;

typedef enum {
    WB_MAP_0,
    WB_MAP_1,
    WB_MAP_2,
    WB_MAP_3,
    WB_MAP_4,
    WB_MAP_5,
    WB_MAP_CNT
} WBMapType;

typedef enum {
    WB_MOVEPATTERN_CRICLE,
    WB_MOVEPATTERN_ZIGZAG,
    WB_MOVEPATTERN_REFLECT,
    WB_MOVEPATTERN_BOUNCE_FLOOR,
    WB_MOVEPATTERN_BOUNCE_CEIL
} WBMovepatternType;

typedef enum {
    WB_POWERUP_NONE     = 0b0000000000000000,
    WB_POWERUP_MAXED    = 0b0000000000000011,

    WB_POWERUP_THRUST   = 0b0000000000000001,
    WB_POWERUP_ANTIGRAV = 0b0000000000000010,

    WB_POWERUP_BEAM     = 0b0000000000000100,
    WB_POWERUP_DOUBLE   = 0b0000000000001000,

    WB_POWERUP_CAT      = 0b0000000000010000,
    //################# = 0b0000000000100000,

    WB_POWERUP_BLAZERS  = 0b0000000001000000,
    //################# = 0b0000000010000000,

    WB_POWERUP_WIZSPRAY = 0b0000000100000000,
    WB_POWERUP_CATSPRAY = 0b0000001000000000,

    WB_POWERUP_BOM      = 0b0000010000000000,
    //################# = 0b0000100000000000,

    WB_POWERUP_SHIELD   = 0b0001000000000000,
    //################# = 0b0010000000000000,
} WBPowerupType;

// Structs

typedef struct {
    int width;
    int height;
    GLuint texture_id;
} WBTexture;

typedef struct {
    WBSpriteType type;
    int atlas_x, atlas_y;
} WBSprite;

typedef struct {
    WBSprite sprite;
    float pos_x, pos_y;
    int health;
} WBEntityHead;

typedef struct {
    WBEntityHead head;
    float vel_x_values[WB_PLAYER_WIZ_VEL_X_CNT];
    float animation_speed_values[WB_PLAYER_WIZ_VEL_X_CNT];
    float vel_y_values[WB_PLAYER_WIZ_VEL_Y_CNT];
    float animation_angle;
    float vel_x_key, vel_x, vel_y_key, vel_y;
    int onscreen_bullet_cnt;
    int pos_x_buffer[WB_PLAYER_CAT_MOVEDELAY_FRAME_CNT];
    int pos_y_buffer[WB_PLAYER_CAT_MOVEDELAY_FRAME_CNT];
    float* collider_angles;
    float collision_angle;
} WBPlayerWiz;

typedef struct {
    WBEntityHead head;
    int onscreen_bullet_cnt;
} WBPlayerCat;

typedef struct {
    WBMovepatternType type;
    float vel_x, vel_y;
} WBMovepattern;

typedef struct {
    WBEntityHead head;
    int attack_period;
} WBEnemy;

typedef struct {
    WBSprite sprite;
    WBParticleType type;
    int pos_x, pos_y;
} WBParticle;

typedef struct {
    WBTexture background;
    WBTexture foreground;
    bool* collider;
} WBMapAtlas;

typedef struct {
    WBMapAtlas atlas;
    int atlas_y;
    bool* collider;
    WBMapType type;
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
    WBGamestateType type;
    int score;
    int enemy_cnt;
    int particle_cnt;
    int powerup_pos;
    int level;
    WBPowerupType powerup_unlocked;
    WBPowerupType powerup_permanent;
} WBGamestate;

typedef struct {
    WBGamestate gamestate;
    WBWindow window;
    WBShader shader;
    WBTexture sprite_atlas;
    WBMap map;
    double last_frame_time;
    unsigned long long frame_cnt;
    WBPlayerWiz player_wiz;
    WBPlayerCat player_cat;
    WBEnemy enemies[WB_ENEMY_CNT_MAX];
    WBParticle particles[WB_PARTICLE_CNT_MAX];
} WBGame;

bool wbWindowInit(WBWindow* window);
void wbWindowLockAspectRatio(WBWindow* window);
bool wbPlayerWizInit(WBPlayerWiz* wiz);
void wbPlayerWizHandleCollision(WBPlayerWiz* wiz, WBMap* map, WBPowerupType movement_powerup);
void wbPlayerWizUpdate(WBPlayerWiz* wiz, WBPowerupType movement_powerup);
void wbShaderInit(WBShader* shader);
bool wbMapInit(WBMap* map);
void wbTextureInit(WBTexture* texture, uint8_t* data, int width, int height);
int wbGameRun();

#endif // WB_H
