#include "wizball.h"

void wbProjectileAppend(WBBufferProjectile* projectile_buffer, WBProjectileType type, float pos_x, float pos_y, float vel_x, float vel_y) {
    WBProjectile* projectile = wbBufferAppend(projectile_buffer, type, pos_x, pos_y);
    projectile->vel_x = vel_x;
    projectile->vel_y = vel_y;
}

void wbProjectileUpdate(WBBufferProjectile* projectile_buffer, WBMap* map, WBWiz* wiz, WBBufferEnemy* enemy_buffer, WBBufferParticle* particle_buffer) {
    WBEnemy* enemies = enemy_buffer->entries;
    WBParticle* particles = particle_buffer->entries;
    WBProjectile* projectile;
    wiz->onscreen_bullet_cnt = 0;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectile_buffer->entries[i];
        if (projectile->head.type == WB_PROJECTILE_NONE) continue;

        projectile->head.pos_x += projectile->vel_x;
        projectile->head.pos_y += projectile->vel_y;

        if (
            projectile->head.pos_x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->head.pos_x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
            projectile->head.pos_y < 0 || projectile->head.pos_y >= WB_MAP_HORIZON_HEIGHT ||
            wbMapGetCollision(map, projectile->head.pos_x, projectile->head.pos_y)
        ) {
            wbBufferRemove(projectile_buffer, i);
            continue;
        }

        int j;
        for (j = 0; j < WB_ENEMY_CNT_MAX; j++) {
            if (
                enemies[j].head.type != WB_ENEMY_NONE &&
                projectile->head.pos_x > enemies[j].head.pos_x - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos_x <= enemies[j].head.pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
                projectile->head.pos_y > enemies[j].head.pos_y - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos_y <= enemies[j].head.pos_y + WB_ENEMY_HITBOX_SIZE / 2
            ) {
                wbEnemyRemove(enemy_buffer, j, particle_buffer);
                wbBufferRemove(projectile_buffer, i);
                break;
            }
        }
        if (j != WB_ENEMY_CNT_MAX) continue;

        for (int j = 0; j < WB_PARTICLE_CNT_MAX; j++) {
            if (
                (WBParticleType)particles[j].head.type == WB_PARTICLE_POWERUP &&
                projectile->head.pos_x > particles[j].head.pos_x - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos_x <= particles[j].head.pos_x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                projectile->head.pos_y > particles[j].head.pos_y - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos_y <= particles[j].head.pos_y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(projectile_buffer, i);
                break;
            }
        }
        if (j != WB_ENEMY_CNT_MAX) continue;

        projectile->head.pos_x += projectile->vel_x;
        projectile->head.pos_y += projectile->vel_y;

        wiz->onscreen_bullet_cnt += (WBProjectileType)projectile->head.type == WB_PROJECTILE_BULLET;
    }
}