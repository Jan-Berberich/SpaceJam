#include "wizball.h"

void wbProjectileAppend(WBProjectile* projectiles, int* projectile_cnt, WBProjectileType type, float pos_x, float pos_y, float vel_x, float vel_y) {
    assert(*projectile_cnt - 1 < WB_PROJECTILE_CNT_MAX);

    while (projectiles->type != WB_PROJECTILE_NONE) {
        projectiles++;
    }

    WBProjectile* projectile = projectiles;
    projectile->pos_x = pos_x;
    projectile->pos_y = pos_y;
    projectile->vel_x = vel_x;
    projectile->vel_y = vel_y;
    projectile->type = type;
    switch (type) {
        case WB_PROJECTILE_BULLET:
        projectile->sprite.atlas_x = WB_PROJECTILE_BULLET_SPRITE_ATLAS_X;
        projectile->sprite.atlas_y = WB_PROJECTILE_BULLET_SPRITE_ATLAS_Y;
        break;
    }
}

void wbProjectileRemove(WBProjectile* projectiles, int* projectile_cnt, int idx) {
     projectiles[idx].type = WB_PROJECTILE_NONE;
    (*projectile_cnt)--;
}

void wbProjectileUpdate(WBProjectile* projectiles, int* projectile_cnt, WBMap* map, WBPlayerWiz* wiz) {
    WBProjectile* projectile;
    wiz->onscreen_bullet_cnt = 0;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectiles[i];
        if (projectile->type == WB_PROJECTILE_NONE) continue;

        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        if (projectile->pos_x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->pos_x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
            projectile->pos_y < 0 || projectile->pos_y >= WB_MAP_HORIZON_HEIGHT ||
            wbMapGetCollision(map, projectile->pos_x, projectile->pos_y)) {
            
            wbProjectileRemove(projectiles, projectile_cnt, i);
            continue;
        }
        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        wiz->onscreen_bullet_cnt += projectile->type == WB_PROJECTILE_BULLET;
    }
}