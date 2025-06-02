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
    (*projectile_cnt)++;
}

void wbProjectileRemove(WBProjectile* projectiles, int* projectile_cnt, int idx) {
     projectiles[idx].type = WB_PROJECTILE_NONE;
    (*projectile_cnt)--;
}

void wbProjectileUpdate(WBProjectile* projectiles, int* projectile_cnt, WBMap* map, WBPlayerWiz* wiz, WBEnemy* enemies, int* enemy_cnt, WBParticle* particles, int* particle_cnt) {
    WBProjectile* projectile;
    wiz->onscreen_bullet_cnt = 0;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectiles[i];
        if (projectile->type == WB_PROJECTILE_NONE) continue;

        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        if (
            projectile->pos_x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->pos_x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
            projectile->pos_y < 0 || projectile->pos_y >= WB_MAP_HORIZON_HEIGHT ||
            wbMapGetCollision(map, projectile->pos_x, projectile->pos_y)
        ) {
            wbProjectileRemove(projectiles, projectile_cnt, i);
            continue;
        }

        for (int j = 0; j < WB_ENEMY_CNT_MAX; j++) {
            if (
                projectile->pos_x > enemies[j].head.pos_x - WB_ENEMY_HITBOX_SIZE / 2 && projectile->pos_x <= enemies[j].head.pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
                projectile->pos_y > enemies[j].head.pos_y - WB_ENEMY_HITBOX_SIZE / 2 && projectile->pos_y <= enemies[j].head.pos_y + WB_ENEMY_HITBOX_SIZE / 2
            ) {
                wbEnemyRemove(enemies, enemy_cnt, j, particles, particle_cnt);
                wbProjectileRemove(projectiles, projectile_cnt, i);
                break;
            }
        }

        for (int j = 0; j < WB_PARTICLE_CNT_MAX; j++) {
            if (
                particles[j].type == WB_PARTICLE_POWERUP &&
                projectile->pos_x > particles[j].pos_x - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->pos_x <= particles[j].pos_x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                projectile->pos_y > particles[j].pos_y - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->pos_y <= particles[j].pos_y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbProjectileRemove(projectiles, projectile_cnt, i);
                break;
            }
        }

        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        wiz->onscreen_bullet_cnt += projectile->type == WB_PROJECTILE_BULLET;
    }
}