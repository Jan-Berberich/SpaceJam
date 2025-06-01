#include "wizball.h"

bool wbPlayerWizInit(WBPlayerWiz* wiz) {
    wiz->collider_angles = malloc(WB_PLAYER_WIZ_COLLISION_ANGLE_CNT * (sizeof *wiz->collider_angles));
    if (!wiz->collider_angles) {
        fprintf(stderr, "Failed to allocate memory for collider_angles");
        return false;
    }
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        wiz->collider_angles[i] = (float)i * M_2PI / WB_PLAYER_WIZ_COLLISION_ANGLE_CNT;
    }

    WBEntityHead* entity = &wiz->head;
    WBSprite* sprite = &entity->sprite;

    sprite->atlas_x = WB_PLAYER_WIZ_SPRITE_ATLAS_X;
    sprite->atlas_y = WB_PLAYER_WIZ_SPRITE_ATLAS_Y;
    entity->health = WB_PLAYER_WIZ_HEALTH_MAX;

    sprite->type = WB_SPRITE_PLAYER;
    entity->pos_x = WB_WINDOW_WIDTH / 2;
    entity->pos_y = WB_PLAYER_WIZ_INIT_POS_Y;
    wiz->vel_x_key = 0.0f;
    wiz->vel_y = 0.0f;

    wiz->vel_x_values[0] = WB_PLAYER_WIZ_VEL_X_0;
    wiz->vel_x_values[1] = WB_PLAYER_WIZ_VEL_X_1;
    wiz->vel_x_values[2] = WB_PLAYER_WIZ_VEL_X_2;
    wiz->vel_x_values[3] = WB_PLAYER_WIZ_VEL_X_3;
    wiz->vel_x_values[4] = WB_PLAYER_WIZ_VEL_X_4;
    wiz->vel_x_values[5] = WB_PLAYER_WIZ_VEL_X_5;
    wiz->vel_x_values[6] = WB_PLAYER_WIZ_VEL_X_6;
    wiz->animation_speed_values[0] = WB_PLAYER_WIZ_ANIMATION_SPEED_0;
    wiz->animation_speed_values[1] = WB_PLAYER_WIZ_ANIMATION_SPEED_1;
    wiz->animation_speed_values[2] = WB_PLAYER_WIZ_ANIMATION_SPEED_2;
    wiz->animation_speed_values[3] = WB_PLAYER_WIZ_ANIMATION_SPEED_3;
    wiz->animation_speed_values[4] = WB_PLAYER_WIZ_ANIMATION_SPEED_4;
    wiz->animation_speed_values[5] = WB_PLAYER_WIZ_ANIMATION_SPEED_5;
    wiz->animation_speed_values[6] = WB_PLAYER_WIZ_ANIMATION_SPEED_6;

    wiz->vel_y_values[0] = WB_PLAYER_WIZ_VEL_Y_0;
    wiz->vel_y_values[1] = WB_PLAYER_WIZ_VEL_Y_1;
    wiz->vel_y_values[2] = WB_PLAYER_WIZ_VEL_Y_2;
    wiz->vel_y_values[3] = WB_PLAYER_WIZ_VEL_Y_3;
    wiz->vel_y_values[4] = WB_PLAYER_WIZ_VEL_Y_4;

    wiz->animation_angle = 0.0f;

    return true;
}

void wbPlayerWizSetCollisionAngle(WBPlayerWiz* wiz, WBMap* map) {
    WBEntityHead* wizh = &wiz->head;
    int xc = wizh->pos_x;
    int yc = wizh->pos_y;
    bool collision;
    int collision_cnt = 0;
    float collision_x = 0.0f;
    float collision_y = 0.0f;

    // Check directions around the player for collision (circle approximation)
    float* angles = wiz->collider_angles;
    int x; int y;
    for (int i = 0; i < WB_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        x = xc + (int)roundf(WB_PLAYER_WIZ_COLLISION_RADIUS * cosf(angles[i]));
        y = yc + (int)roundf(WB_PLAYER_WIZ_COLLISION_RADIUS * sinf(angles[i]));
        collision = map->collider[y * map->atlas.foreground.width + x];
        collision_cnt += collision;
        collision_x += collision * (x - xc);
        collision_y += collision * (y - yc);
    }

    wiz->collision_angle = atan2f(collision_y / collision_cnt, collision_x / collision_cnt);
}

void wbPlayerWizHandleCollision(WBPlayerWiz* wiz, WBMap* map, WBPowerupType movement_powerup) {
    int pos_y = roundf(wiz->head.pos_y / WB_SUBPIXEL_Y_CNT) * WB_SUBPIXEL_Y_CNT;
    bool map_ceil_collision = pos_y - WB_PLAYER_WIZ_COLLISION_RADIUS < WB_MAP_CEIL_HEIGHT;
    bool map_floor_collision = pos_y + WB_PLAYER_WIZ_COLLISION_RADIUS > WB_MAP_FLOOR_HEIGHT;
    if (map_ceil_collision || map_floor_collision) {
        wiz->collision_angle = M_2PI / 4.0f * (map_floor_collision ? 1.0f : -1.0f);
    }
    else {
        wbPlayerWizSetCollisionAngle(wiz, map);
    }

    if (!isnan(wiz->collision_angle)) {
        float collision_x = cosf(wiz->collision_angle);
        float collision_y = sinf(wiz->collision_angle);
        if (fabsf(collision_x) > WB_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN) {
            wiz->vel_x_key = -fabsf(wiz->vel_x_key) * fsgnf(collision_x);
        }
        if (fabsf(collision_y) > 0.01f) {
            if (movement_powerup == WB_POWERUP_NONE || movement_powerup == WB_POWERUP_THRUST) {
                wiz->vel_y += WB_PLAYER_WIZ_GRAVITY;
                wiz->vel_y = -fabsf(wiz->vel_y) * fsgnf(collision_y);
            }
            else {
                if (map_ceil_collision || map_floor_collision) {
                    wiz->vel_y_key = -fsgnf(collision_y);
                    wiz->vel_y_key += WB_PLAYER_WIZ_ACC_Y * fsgnf(collision_y);
                }
                else {
                    wiz->vel_y_key -= WB_PLAYER_WIZ_ACC_Y * fsgnf(collision_y);
                    wiz->vel_y_key = -fabsf(wiz->vel_y_key) * fsgnf(collision_y);
                }
            }
        }
    }
}

void wbPlayerWizUpdate(WBPlayerWiz* wiz, WBPowerupType movement_powerup) {
    if (movement_powerup == WB_POWERUP_NONE && !isnan(wiz->collision_angle) || movement_powerup != WB_POWERUP_NONE) {
        wiz->vel_x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))] * WB_VELOCITY_FACTOR;
    }
    if (movement_powerup == WB_POWERUP_ANTIGRAV) {
        wiz->vel_y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))] * WB_VELOCITY_FACTOR;
    }
    if (movement_powerup == WB_POWERUP_NONE || movement_powerup == WB_POWERUP_THRUST) {
        wiz->vel_y += WB_PLAYER_WIZ_GRAVITY;
    }
    wiz->head.pos_x += wiz->vel_x;
    wiz->head.pos_y += wiz->vel_y;

    wiz->animation_angle += fsgnf(wiz->vel_x_key) * wiz->animation_speed_values[(int)roundf(fabsf(wiz->vel_x_key))];
    wiz->animation_angle += wiz->animation_angle <  -0.5f ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
    wiz->animation_angle -= wiz->animation_angle >= -0.5f + WB_PLAYER_WIZ_ANIMATION_FRAME_CNT ? WB_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
}