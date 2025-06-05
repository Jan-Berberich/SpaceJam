#include "wizball.h"

void wbProjectileBufferInit(WBProjectileBuffer* projectile_buffer) {
    projectile_buffer->head.cnt = 0;
    projectile_buffer->head.type = WB_BUFFER_PROJECTILE;
    WBProjectile* projectiles = projectile_buffer->entries;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectiles[i].head.type = WB_PROJECTILE_NONE;
    }

    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_BULLET  ] = WB_PROJECTILE_BULLET_SPRITE_ATLAS_X;
    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_BLAZER  ] = WB_PROJECTILE_BLAZER_SPRITE_ATLAS_X;
    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_SPRAY_NW] = WB_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_X;
    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_SPRAY_N ] = WB_PROJECTILE_SPRAY_N_SPRITE_ATLAS_X;
    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_SPRAY_NE] = WB_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_X;
    projectile_buffer->sprite_atlas_x[WB_PROJECTILE_BLINKER ] = WB_PROJECTILE_BLINKER_SPRITE_ATLAS_X;

    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_BULLET  ] = WB_PROJECTILE_BULLET_SPRITE_ATLAS_Y;
    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_BLAZER  ] = WB_PROJECTILE_BLAZER_SPRITE_ATLAS_Y;
    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_SPRAY_NW] = WB_PROJECTILE_SPRAY_NW_SPRITE_ATLAS_Y;
    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_SPRAY_N ] = WB_PROJECTILE_SPRAY_N_SPRITE_ATLAS_Y;
    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_SPRAY_NE] = WB_PROJECTILE_SPRAY_NE_SPRITE_ATLAS_Y;
    projectile_buffer->sprite_atlas_y[WB_PROJECTILE_BLINKER ] = WB_PROJECTILE_BLINKER_SPRITE_ATLAS_Y;
}

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
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectile_buffer->entries[i];
        if (projectile->head.type == WB_PROJECTILE_NONE) continue;

        projectile->head.pos.x += projectile->vel.x;
        projectile->head.pos.y += projectile->vel.y;

        if (
            projectile->head.pos.x < map->view_center_x - WB_MAP_VIEW_WIDTH / 2 || projectile->head.pos.x >= map->view_center_x + WB_MAP_VIEW_WIDTH / 2 ||
            projectile->head.pos.y < 0 || projectile->head.pos.y >= WB_MAP_HORIZON_HEIGHT ||
            wbMapGetCollision(map, roundf(projectile->head.pos.x), roundf(projectile->head.pos.y))
        ) {
            wbBufferRemove(projectile_buffer, i);
            continue;
        }

        int j;
        for (j = 0; j < WB_ENEMY_CNT_MAX; j++) {
            if (
                enemies[j].head.type != WB_ENEMY_NONE &&
                projectile->head.pos.x > enemies[j].head.pos.x - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.x <= enemies[j].head.pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
                projectile->head.pos.y > enemies[j].head.pos.y - WB_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.y <= enemies[j].head.pos.y + WB_ENEMY_HITBOX_SIZE / 2
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
                projectile->head.pos.x > particles[j].head.pos.x - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.x <= particles[j].head.pos.x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                projectile->head.pos.y > particles[j].head.pos.y - WB_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.y <= particles[j].head.pos.y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(projectile_buffer, i);
                break;
            }
        }
        if (j != WB_PARTICLE_CNT_MAX) continue;

        projectile->head.pos.x += projectile->vel.x;
        projectile->head.pos.y += projectile->vel.y;

        wiz->onscreen_bullet_cnt += (WBProjectileType)projectile->head.type == WB_PROJECTILE_BULLET;
    }
}