#include "wizball.h"

void wbPlayerWizInit(WBWiz* wiz, float pos_x_min, float pos_x_max) {
    for (int i = 0; i < WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        wiz->collider_angles[i] = (float)i * M_2PI / WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT;
    }
    wiz->health = WB_GAMERULE_PLAYER_WIZ_HEALTH_MAX;
    wiz->pos.x = randfin(time(NULL), pos_x_min, pos_x_max);
    wiz->pos.y = WB_GAMERULE_PLAYER_WIZ_SPAWN_POS_Y;
    wiz->vel_x_key = 0.0f;
    wiz->vel.x = 0.0f;
    wiz->vel_y_key = 0.0f;
    wiz->vel.y = WB_GAMERULE_PLAYER_WIZ_SPAWN_VEL_Y;
    wiz->collision_vec.x = 0.0f;
    wiz->collision_vec.y = 0.0f;
    wiz->next_bullet_direction = WB_DIRECTION_POSITIVE;
    wiz->next_spray_direction = WB_DIRECTION_NEGATIVE;
    wiz->facing = WB_DIRECTION_POSITIVE;

    wiz->vel_x_values[0] = WB_GAMERULE_PLAYER_WIZ_VEL_X_0;
    wiz->vel_x_values[1] = WB_GAMERULE_PLAYER_WIZ_VEL_X_1;
    wiz->vel_x_values[2] = WB_GAMERULE_PLAYER_WIZ_VEL_X_2;
    wiz->vel_x_values[3] = WB_GAMERULE_PLAYER_WIZ_VEL_X_3;
    wiz->vel_x_values[4] = WB_GAMERULE_PLAYER_WIZ_VEL_X_4;
    wiz->vel_x_values[5] = WB_GAMERULE_PLAYER_WIZ_VEL_X_5;
    wiz->vel_x_values[6] = WB_GAMERULE_PLAYER_WIZ_VEL_X_6;
    wiz->vel_x_values[7] = WB_GAMERULE_PLAYER_WIZ_VEL_X_7;
    wiz->animation_speed_values[0] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_0;
    wiz->animation_speed_values[1] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_1;
    wiz->animation_speed_values[2] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_2;
    wiz->animation_speed_values[3] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_3;
    wiz->animation_speed_values[4] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_4;
    wiz->animation_speed_values[5] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_5;
    wiz->animation_speed_values[6] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_6;
    wiz->animation_speed_values[7] = WB_GRAPHIC_PLAYER_WIZ_ANIMATION_SPEED_7;

    wiz->vel_y_values[0] = WB_GAMERULE_PLAYER_WIZ_VEL_Y_0;
    wiz->vel_y_values[1] = WB_GAMERULE_PLAYER_WIZ_VEL_Y_1;
    wiz->vel_y_values[2] = WB_GAMERULE_PLAYER_WIZ_VEL_Y_2;
    wiz->vel_y_values[3] = WB_GAMERULE_PLAYER_WIZ_VEL_Y_3;
    wiz->vel_y_values[4] = WB_GAMERULE_PLAYER_WIZ_VEL_Y_4;

    wiz->animation_angle = 0.0f;
}

void wbPlayerCatInit(WBCat* cat) {
    cat->health = WB_GAMERULE_PLAYER_CAT_HEALTH_MAX;
    cat->rest_offset_x = WB_GAMERULE_PLAYER_CAT_REST_OFFSET_X;
    cat->next_spray_direction = WB_DIRECTION_NEGATIVE;
    cat->pos_y_buffer_idx = 0;
}

void wbPlayerWizSetCollisionVec(WBWiz* wiz, WBMap* map, int level) {
    int xc = roundf(wiz->pos.x);
    int yc = roundf(wiz->pos.y);
    bool is_collision;
    int collision_cnt = 0;
    WBVec2f collision_vec = {0.0f};

    // Check directions around the player for collision (circle approximation)
    float* angles = wiz->collider_angles;
    int x; int y;
    for (int i = 0; i < WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_CNT; i++) {
        x = xc + roundf(WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS * cosf(angles[i]));
        y = yc + roundf(WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS * sinf(angles[i]));
        is_collision = wbMapGetCollision(map, x, y, level);
        collision_cnt += is_collision;
        collision_vec.x += is_collision * (x - xc);
        collision_vec.y += is_collision * (y - yc);
    }

    float collision_vec_normalizer = (collision_vec.x || collision_vec.y) ? 
        rsqrtf(collision_vec.x * collision_vec.x + collision_vec.y * collision_vec.y) : 1.0f;
    wiz->collision_vec.x = collision_vec.x * collision_vec_normalizer;
    wiz->collision_vec.y = collision_vec.y * collision_vec_normalizer;

}

void wbPlayerWizHandleCollision(WBWiz* wiz, WBMap* map, WBGamestate* gamestate) {
    static double collision_time = 1.0 / WB_GAMERULE_PLAYER_WIZ_HANDLE_COLLISION_SPEED;
    static double timer = 1.0 / WB_GAMERULE_PLAYER_WIZ_HANDLE_COLLISION_SPEED;
    if (timer < collision_time) {
        timer += gamestate->delta_time;
        return;
    }
    timer += gamestate->delta_time;
    timer -= collision_time;

    int pos_y = roundf(wiz->pos.y / WB_GRAPHIC_SUBPIXEL_CNT) * WB_GRAPHIC_SUBPIXEL_CNT;
    bool map_ceil_collision = pos_y - WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS < WB_GAMERULE_MAP_CEIL_HEIGHT;
    bool map_floor_collision = pos_y + WB_GAMERULE_PLAYER_WIZ_COLLISION_RADIUS > WB_GAMERULE_MAP_FLOOR_HEIGHT;
    if (map_ceil_collision || map_floor_collision) {
        wiz->collision_vec.x = 0.0f;
        wiz->collision_vec.y = map_floor_collision ? 1.0f : -1.0f;
    }
    else {
        wbPlayerWizSetCollisionVec(wiz, map, gamestate->level);
    }

    if (wiz->collision_vec.x || wiz->collision_vec.y) {
        if (fabsf(wiz->collision_vec.x) > WB_GAMERULE_PLAYER_WIZ_COLLISION_ANGLE_FLAT_SIN) {
            wiz->vel_x_key = -fabsf(wiz->vel_x_key) * fsgnf(wiz->collision_vec.x);
        }
        if (fabsf(wiz->collision_vec.y) > 0.01f) {
            if (!(gamestate->powerup.unlocked & WB_POWERUP_ANTIGRAV)) {
                wiz->vel.y += WB_GAMERULE_GRAVITY * gamestate->delta_time;
                wiz->vel.y = -fsgnf(wiz->collision_vec.y) * fabsf(wiz->vel.y);
            }
            else {
                if (map_ceil_collision || map_floor_collision) {
                    wiz->vel_y_key = -fsgnf(wiz->collision_vec.y);
                    wiz->vel_y_key += fsgnf(wiz->collision_vec.y) * WB_GAMERULE_PLAYER_WIZ_ACC_Y * collision_time;
                }
                else {
                    wiz->vel_y_key -= fsgnf(wiz->collision_vec.y) * WB_GAMERULE_PLAYER_WIZ_ACC_Y * collision_time;
                    wiz->vel_y_key = -fabsf(wiz->vel_y_key) * fsgnf(wiz->collision_vec.y);
                }
            }
        }
    }
}

void wbPlayerWizUpdate(WBWiz* wiz, WBMap* map, WBGamestate* gamestate) {
    WBTexture* map_atlas = &map->graphic_handle->background_atlas;
    WBView* view = &map->view;
    if ((wiz->collision_vec.x || wiz->collision_vec.y) || (gamestate->powerup.unlocked & WB_POWERUP_THRUST) || (gamestate->powerup.unlocked & WB_POWERUP_ANTIGRAV)) {
        wiz->vel.x = fsgnf(wiz->vel_x_key) * wiz->vel_x_values[(int)roundf(fabsf(wiz->vel_x_key))];
    }
    if (gamestate->powerup.unlocked & WB_POWERUP_ANTIGRAV) {
        wiz->vel.y = fsgnf(wiz->vel_y_key) * wiz->vel_y_values[(int)roundf(fabsf(wiz->vel_y_key))];
    }
    if (!(gamestate->powerup.unlocked & WB_POWERUP_ANTIGRAV)) {
        wiz->vel.y += WB_GAMERULE_GRAVITY * gamestate->delta_time;
    }
    wiz->facing = fsgnf(wiz->vel_x_key) ? fsgnf(wiz->vel_x_key) : wiz->facing;

    wiz->pos.x += wiz->vel.x * gamestate->delta_time;
    wiz->pos.y += wiz->vel.y * gamestate->delta_time;

    wiz->animation_angle += fsgnf(wiz->vel_x_key) * wiz->animation_speed_values[(int)roundf(fabsf(wiz->vel_x_key))] * gamestate->delta_time;
    wiz->animation_angle += wiz->animation_angle <  -0.5f ? WB_GRAPHIC_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;
    wiz->animation_angle -= wiz->animation_angle >= -0.5f + WB_GRAPHIC_PLAYER_WIZ_ANIMATION_FRAME_CNT ? WB_GRAPHIC_PLAYER_WIZ_ANIMATION_FRAME_CNT : 0;

    view->center_x = wiz->pos.x;
    view->center_x = fmaxf(view->center_x, WB_GRAPHIC_MAP_VIEW_WIDTH / 2);
    view->center_x = fminf(view->center_x, map_atlas->width - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + 1);
}

void wbPlayerCatUpdate(WBCat* cat, WBWiz* wiz, WBMap* map, WBGamestate* gamestate) {
    WBTexture* map_atlas = &map->graphic_handle->background_atlas;
    WBView* view = &map->view;
    if (!(gamestate->powerup.unlocked & WB_POWERUP_CAT)) {
        cat->pos.x = view->center_x - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + WB_GAMERULE_PLAYER_CAT_WIDTH / 2;
        cat->pos.y = wiz->pos.y < (float)map_atlas->height / WB_MAP_CNT / 2 ?
                   - WB_GRAPHIC_SPRITE_SIZE : (float)map_atlas->height / WB_MAP_CNT + WB_GRAPHIC_SPRITE_SIZE;
        return;
    }

    cat->pos.x += gamestate->delta_time * (cat->vel.x + wiz->vel.x * (wiz->pos.x == map->view.center_x));
    cat->pos.y += gamestate->delta_time *  cat->vel.y;

    cat->pos.x = fmaxf(cat->pos.x, view->center_x - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 + WB_GAMERULE_PLAYER_CAT_WIDTH / 2);
    cat->pos.x = fminf(cat->pos.x, view->center_x + WB_GRAPHIC_MAP_VIEW_WIDTH / 2 - WB_GAMERULE_PLAYER_CAT_WIDTH / 2);
    cat->pos.y = fmaxf(cat->pos.y, WB_GAMERULE_MAP_CEIL_HEIGHT  + WB_GAMERULE_PLAYER_CAT_CEIL_OFFSET);
    cat->pos.y = fminf(cat->pos.y, WB_GAMERULE_MAP_FLOOR_HEIGHT - WB_GAMERULE_PLAYER_CAT_FLOOR_OFFSET);

    if (cat->retreat) {
        cat->rest_offset_x -= fminf(fabsf(cat->rest_offset_x + WB_GAMERULE_PLAYER_CAT_REST_OFFSET_X * wiz->facing),
                                    WB_GAMERULE_PLAYER_CAT_REST_OFFSET_VEL * gamestate->delta_time) * wiz->facing;
        cat->pos.x += fsgnf(wiz->pos.x + cat->rest_offset_x - cat->pos.x)
                    * fminf(fabsf(wiz->pos.x + cat->rest_offset_x - cat->pos.x), WB_GAMERULE_PLAYER_CAT_VEL * gamestate->delta_time);
        cat->pos.y += fsgnf(cat->pos_y_buffer[cat->pos_y_buffer_idx] - cat->pos.y)
                    * fminf(fabsf(cat->pos_y_buffer[cat->pos_y_buffer_idx] - cat->pos.y), WB_GAMERULE_PLAYER_CAT_VEL * gamestate->delta_time);
        cat->facing = fsgnf(wiz->pos.x - cat->pos.x) ? fsgnf(wiz->pos.x - cat->pos.x) : cat->facing;
    }
    cat->facing = cat->vel.x < 0 ? WB_DIRECTION_NEGATIVE : cat->vel.x > 0 ? WB_DIRECTION_POSITIVE : cat->facing;
}