#include "wizball.h"

bool wbPlayerWizInit(WBWiz* wiz, float pos_x_min, float pos_x_max) {
    wiz->collider_angles = malloc(WB_PLAYER_WIZ_COLLISION_ANGLE_CNT * (sizeof *wiz->collider_angles));
    if (!wiz->collider_angles) {
        fprintf(stderr, "Failed to allocate memory for collider_angles");
        return false;
    }
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        wiz->collider_angles[i] = (float)i * M_2PI / WB_PLAYER_WIZ_COLLISION_ANGLE_CNT;
    }

    wiz->health = WB_PLAYER_WIZ_HEALTH_MAX;
    wiz->pos.x = randfin(time(NULL), pos_x_min, pos_x_max);
    wiz->pos.y = WB_PLAYER_WIZ_INIT_POS_Y;
    wiz->vel_x_key = 0.0f;
    wiz->vel.x = 0.0f;
    wiz->vel_y_key = 0.0f;
    wiz->vel.y = 0.0f;
    wiz->next_bullet_direction = WB_DIRECTION_POSITIVE;
    wiz->next_spray_direction = WB_DIRECTION_NEGATIVE;
    wiz->facing = WB_DIRECTION_POSITIVE;

    wiz->vel_x_values[0] = WB_PLAYER_WIZ_VEL_X_0;
    wiz->vel_x_values[1] = WB_PLAYER_WIZ_VEL_X_1;
    wiz->vel_x_values[2] = WB_PLAYER_WIZ_VEL_X_2;
    wiz->vel_x_values[3] = WB_PLAYER_WIZ_VEL_X_3;
    wiz->vel_x_values[4] = WB_PLAYER_WIZ_VEL_X_4;
    wiz->vel_x_values[5] = WB_PLAYER_WIZ_VEL_X_5;
    wiz->vel_x_values[6] = WB_PLAYER_WIZ_VEL_X_6;
    wiz->vel_x_values[7] = WB_PLAYER_WIZ_VEL_X_7;
    wiz->animation_speed_values[0] = WB_PLAYER_WIZ_ANIMATION_SPEED_0;
    wiz->animation_speed_values[1] = WB_PLAYER_WIZ_ANIMATION_SPEED_1;
    wiz->animation_speed_values[2] = WB_PLAYER_WIZ_ANIMATION_SPEED_2;
    wiz->animation_speed_values[3] = WB_PLAYER_WIZ_ANIMATION_SPEED_3;
    wiz->animation_speed_values[4] = WB_PLAYER_WIZ_ANIMATION_SPEED_4;
    wiz->animation_speed_values[5] = WB_PLAYER_WIZ_ANIMATION_SPEED_5;
    wiz->animation_speed_values[6] = WB_PLAYER_WIZ_ANIMATION_SPEED_6;
    wiz->animation_speed_values[7] = WB_PLAYER_WIZ_ANIMATION_SPEED_7;

    wiz->vel_y_values[0] = WB_PLAYER_WIZ_VEL_Y_0;
    wiz->vel_y_values[1] = WB_PLAYER_WIZ_VEL_Y_1;
    wiz->vel_y_values[2] = WB_PLAYER_WIZ_VEL_Y_2;
    wiz->vel_y_values[3] = WB_PLAYER_WIZ_VEL_Y_3;
    wiz->vel_y_values[4] = WB_PLAYER_WIZ_VEL_Y_4;

    wiz->animation_angle = 0.0f;

    return true;
}

void wbPlayerWizSetCollisionAngle(WBWiz* wiz, WBMap* map) {
    int xc = roundf(wiz->pos.x);
    int yc = roundf(wiz->pos.y);
    bool is_collision;
    int collision_cnt = 0;
    WBVec2f collision_vec = {0.0f};

    // Check directions around the player for collision (circle approximation)
    float* angles = wiz->collider_angles;
    int x; int y;
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        x = xc + roundf((float)WB_PLAYER_WIZ_COLLISION_RADIUS * cosf(angles[i]));
        y = yc + roundf((float)WB_PLAYER_WIZ_COLLISION_RADIUS * sinf(angles[i]));
        is_collision = wbMapGetCollision(map, x, y);
        collision_cnt += is_collision;
        collision_vec.x += is_collision * (x - xc);
        collision_vec.y += is_collision * (y - yc);
    }

    wiz->collision_angle = atan2f(collision_vec.y / collision_cnt, collision_vec.x / collision_cnt);
}

void wbPlayerWizHandleCollision(WBWiz* wiz, WBMap* map, WBPowerupType powerup_unlocked) {
    int pos_y = roundf(wiz->pos.y / WB_SUBPIXEL_CNT) * WB_SUBPIXEL_CNT;
    bool map_ceil_collision = pos_y - WB_PLAYER_WIZ_COLLISION_RADIUS < WB_MAP_CEIL_HEIGHT;
    bool map_floor_collision = pos_y + WB_PLAYER_WIZ_COLLISION_RADIUS > WB_MAP_FLOOR_HEIGHT;
    if (map_ceil_collision || map_floor_collision) {
        wiz->collision_angle = M_2PI / 4.0f * (map_floor_collision ? 1.0f : -1.0f);
    }
    else {
        wbPlayerWizSetCollisionAngle(wiz, map);
    }

    if (!isnan(wiz->collision_angle)) {
        WBVec2f collision = {cosf(wiz->collision_angle), sinf(wiz->collision_angle)};
        if (fabsf(collision.x) > WB_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN) {
            wiz->vel_x_key = -fabsf(wiz->vel_x_key) * fsgnf(collision.x);
        }
        if (fabsf(collision.y) > 0.01f) {
            if (!(powerup_unlocked & WB_POWERUP_ANTIGRAV)) {
                wiz->vel.y += WB_PLAYER_WIZ_GRAVITY;
                wiz->vel.y = -fabsf(wiz->vel.y) * fsgnf(collision.y);
            }
            else {
                if (map_ceil_collision || map_floor_collision) {
                    wiz->vel_y_key = -fsgnf(collision.y);
                    wiz->vel_y_key += WB_PLAYER_WIZ_ACC_Y * fsgnf(collision.y);
                }
                else {
                    wiz->vel_y_key -= WB_PLAYER_WIZ_ACC_Y * fsgnf(collision.y);
                    wiz->vel_y_key = -fabsf(wiz->vel_y_key) * fsgnf(collision.y);
                }
            }
        }
    }
}

void wbPlayerWizUpdate(WBWiz* wiz, WBPowerupType powerup_unlocked) {
    if (!isnan(wiz->collision_angle) || (powerup_unlocked & WB_POWERUP_THRUST) || (powerup_unlocked & WB_POWERUP_ANTIGRAV)) {
        wiz->vel.x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
    }
    if (powerup_unlocked & WB_POWERUP_ANTIGRAV) {
        wiz->vel.y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
    }
    if (!(powerup_unlocked & WB_POWERUP_ANTIGRAV)) {
        wiz->vel.y += WB_PLAYER_WIZ_GRAVITY;
    }
    wiz->pos.x += wiz->vel.x;
    wiz->pos.y += wiz->vel.y;

    wiz->animation_angle += fsgnf(wiz->vel_x_key) * wiz->animation_speed_values[(int)roundf(fabsf(wiz->vel_x_key))];
    wiz->animation_angle += wiz->animation_angle <  -0.5f ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
    wiz->animation_angle -= wiz->animation_angle >= -0.5f + WB_PLAYER_WIZ_ANIMATION_FRAME_CNT ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
}