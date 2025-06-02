#include "wizball.h"

void wbProjectileAppend(WBBufferProjectile* projectile_buffer, WBProjectileType type, float pos_x, float pos_y, float vel_x, float vel_y) {
    assert(projectile_buffer->head.cnt - 1 < WB_PROJECTILE_CNT_MAX);

    WBProjectile* projectiles = projectile_buffer->entries;
    while (projectiles->type != WB_PROJECTILE_NONE) {
        projectiles++;
    }

    WBProjectile* projectile = projectiles;
    projectile->pos_x = pos_x;
    projectile->pos_y = pos_y;
    projectile->vel_x = vel_x;
    projectile->vel_y = vel_y;
    projectile->type = type;
    projectile_buffer->head.cnt++;
}

void wbProjectileUpdate(WBBufferProjectile* projectile_buffer, WBMap* map, WBWiz* wiz, WBBufferEnemy* enemy_buffer, WBBufferParticle* particle_buffer) {
    WBEnemy* enemies = enemy_buffer->entries;
    WBParticle* particles = particle_buffer->entries;
    WBProjectile* projectile;
    wiz->onscreen_bullet_cnt = 0;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectile_buffer->entries[i];
        if (projectile->type == WB_PROJECTILE_NONE) continue;

        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        if (
            projectile->pos_x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->pos_x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
            projectile->pos_y < 0 || projectile->pos_y >= WB_MAP_HORIZON_HEIGHT ||
            wbMapGetCollision(map, projectile->pos_x, projectile->pos_y)
        ) {
            wbBufferRemove(&projectile_buffer->head, i);
            continue;
        }

        for (int j = 0; j < WB_ENEMY_CNT_MAX; j++) {
            if (
                projectile->pos_x > enemies[j].pos_x - WB_ENEMY_HITBOX_SIZE / 2 && projectile->pos_x <= enemies[j].pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
                projectile->pos_y > enemies[j].pos_y - WB_ENEMY_HITBOX_SIZE / 2 && projectile->pos_y <= enemies[j].pos_y + WB_ENEMY_HITBOX_SIZE / 2
            ) {
                wbEnemyRemove(enemy_buffer, j, particle_buffer);
                wbBufferRemove(&projectile_buffer->head, i);
                break;
            }
        }

        for (int j = 0; j < WB_PARTICLE_CNT_MAX; j++) {
            if (
                particles[j].type == WB_PARTICLE_POWERUP &&
                projectile->pos_x > particles[j].pos_x - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->pos_x <= particles[j].pos_x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                projectile->pos_y > particles[j].pos_y - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->pos_y <= particles[j].pos_y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(&projectile_buffer->head, i);
                break;
            }
        }

        projectile->pos_x += projectile->vel_x;
        projectile->pos_y += projectile->vel_y;

        wiz->onscreen_bullet_cnt += projectile->type == WB_PROJECTILE_BULLET;
    }
}