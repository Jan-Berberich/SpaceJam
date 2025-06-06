#include "wizball.h"

void wbProjectileAppend(WBProjectileBuffer* projectile_buffer, WBProjectileType type, WBVec2f* pos, WBVec2f* vel) {
    WBProjectile* projectile = wbBufferAppend(projectile_buffer, type, pos);
    projectile->vel.x = vel->x;
    projectile->vel.y = vel->y;
}

void wbProjectileUpdate(WBProjectileBuffer* projectile_buffer, WBMap* map, WBWiz* wiz, WBBufferEnemy* enemy_buffer, WBBufferParticle* particle_buffer) {
    WBEnemy* enemies = enemy_buffer->entries;
    WBParticle* particles = particle_buffer->entries;
    WBProjectile* projectile;
    wiz->onscreen_bullet_cnt = 0;
    wiz->onscreen_beam = false;
    wiz->onscreen_spray = false;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectile_buffer->entries[i];
        switch (projectile->head.type) {
            case WB_PROJECTILE_NONE: continue;

            case WB_PROJECTILE_BEAM:
            // render both parts from one projectile
            // use color_key for offset_u and (color_key + frame_cnt) % WB_PROJECTILE_BEAM_ANIMATION_COLOR_CNT
            wiz->onscreen_beam = true;
            projectile->head.pos = wiz->pos;
            if ((int)(projectile->head.color_key / WB_PROJECTILE_BEAM_ANIMATION_COLOR_SPEED * WB_PROJECTILE_BEAM_ANIMATION_SPEED) >= WB_PROJECTILE_BEAM_ANIMATION_FRAME_CNT) {
                wbBufferRemove(projectile_buffer, i);
                continue;
            } projectile->head.color_key += WB_PROJECTILE_BEAM_ANIMATION_COLOR_SPEED;
            for (int j = 0; j < WB_ENEMY_CNT_MAX; j++) {
                if (enemies[j].head.type != WB_ENEMY_NONE &&
                    projectile->head.pos.x > enemies[j].head.pos.x - WB_ENEMY_HITBOX_SIZE / 2                                      && projectile->head.pos.x <= enemies[j].head.pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > enemies[j].head.pos.y - WB_ENEMY_HITBOX_SIZE / 2 - WB_PROJECTILE_BEAM_HITBOX_SIZE / 2 && projectile->head.pos.y <= enemies[j].head.pos.y + WB_ENEMY_HITBOX_SIZE / 2 + WB_PROJECTILE_BEAM_HITBOX_SIZE / 2
                ) {
                    wbEnemyRemove(enemy_buffer, j, particle_buffer);
                }
            }
            break;

            default:
            projectile->head.pos.x += projectile->vel.x;
            projectile->head.pos.y += projectile->vel.y;
            if (projectile->head.pos.x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->head.pos.x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
                projectile->head.pos.y < 0 || projectile->head.pos.y >= WB_MAP_HORIZON_HEIGHT ||
                wbMapGetCollision(map, roundf(projectile->head.pos.x), roundf(projectile->head.pos.y))
            ) {
                wbBufferRemove(projectile_buffer, i);
                continue;
            }
            int j;
            for (j = 0; j < WB_ENEMY_CNT_MAX; j++) {
                if (enemies[j].head.type != WB_ENEMY_NONE &&
                    projectile->head.pos.x > enemies[j].head.pos.x - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.x <= enemies[j].head.pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > enemies[j].head.pos.y - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.y <= enemies[j].head.pos.y + WB_ENEMY_HITBOX_SIZE / 2
                ) {
                    wbEnemyRemove(enemy_buffer, j, particle_buffer);
                    wbBufferRemove(projectile_buffer, i);
                    break;
                }
            } if (j != WB_ENEMY_CNT_MAX) continue;
            for (int j = 0; j < WB_PARTICLE_CNT_MAX; j++) {
                if ((WBParticleType)particles[j].head.type == WB_PARTICLE_POWERUP &&
                    projectile->head.pos.x > particles[j].head.pos.x - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.x <= particles[j].head.pos.x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > particles[j].head.pos.y - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.y <= particles[j].head.pos.y + WB_PARTICLE_HITBOX_SIZE / 2
                ) {
                    wbBufferRemove(projectile_buffer, i);
                    break;
                }
            } if (j != WB_PARTICLE_CNT_MAX) continue;
            wiz->onscreen_bullet_cnt += (WBProjectileType)projectile->head.type == WB_PROJECTILE_BULLET;
            wiz->onscreen_spray       = (WBProjectileType)projectile->head.type == WB_PROJECTILE_SPRAY_NW
                                     || (WBProjectileType)projectile->head.type == WB_PROJECTILE_SPRAY_N
                                     || (WBProjectileType)projectile->head.type == WB_PROJECTILE_SPRAY_NE;
        }
    }
}