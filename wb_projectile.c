#include "wizball.h"

int wbProjectileAppend(WBProjectileBuffer* projectile_buffer, WBProjectileType type, WBVec2f* pos, WBVec2f* vel) {
    int idx = wbBufferAppend(projectile_buffer, type, pos);
    WBProjectile* projectile = &projectile_buffer->entries[idx];
    projectile->vel.x = vel->x;
    projectile->vel.y = vel->y;
    return idx;
}

void wbProjectileUpdate(WBProjectileBuffer* projectile_buffer, WBMap* map, WBWiz* wiz, WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBView* view = &map->view;
    WBEnemy* enemies = enemy_buffer->entries;
    WBParticle* particles = particle_buffer->entries;
    WBProjectile* projectile;
    view->bullet_wiz_cnt = 0;
    view->bullet_cat_cnt = 0;
    view->beam = false;
    view->spray = false;
    for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
        projectile = &projectile_buffer->entries[i];
        switch (projectile->head.type) {
            case WB_PROJECTILE_NONE: continue;

            case WB_PROJECTILE_BEAM:
            // render both parts from one projectile
            view->beam = true;
            projectile->head.pos = wiz->pos;
            projectile->head.color_key += WB_GRAPHIC_PROJECTILE_BEAM_COLORPALLET_SPEED;
            projectile->head.color_key -= projectile->head.color_key >= WB_GRAPHIC_PROJECTILE_BEAM_COLORPALLET_CNT ? WB_GRAPHIC_PROJECTILE_BEAM_COLORPALLET_CNT : 0;
            projectile->head.animation_key += WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_SPEED;
            if (projectile->head.animation_key >= WB_GRAPHIC_PROJECTILE_BEAM_ANIMATION_FRAME_CNT) {
                wbBufferRemove(projectile_buffer, i);
                continue;
            }
            for (int j = 0; j < WB_ENEMY_CNT_MAX; j++) {
                if (enemies[j].head.type != WB_ENEMY_NONE &&
                    projectile->head.pos.x > enemies[j].head.pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2                                      && projectile->head.pos.x <= enemies[j].head.pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > enemies[j].head.pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 - WB_GAMERULE_PROJECTILE_BEAM_HITBOX_SIZE / 2 && projectile->head.pos.y <= enemies[j].head.pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 + WB_GAMERULE_PROJECTILE_BEAM_HITBOX_SIZE / 2
                ) {
                    wbEnemyRemove(enemy_buffer, j, particle_buffer, gamestate, sound);
                }
            }
            break;

            default:
            projectile->head.pos.x += projectile->vel.x;
            projectile->head.pos.y += projectile->vel.y;
            int j;
            // enemy hit?
            for (j = 0; j < WB_ENEMY_CNT_MAX; j++) {
                if (enemies[j].head.type != WB_ENEMY_NONE &&
                    projectile->head.pos.x > enemies[j].head.pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.x <= enemies[j].head.pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > enemies[j].head.pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && projectile->head.pos.y <= enemies[j].head.pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2) {
                    
                    wbEnemyRemove(enemy_buffer, j, particle_buffer, gamestate, sound);
                    wbBufferRemove(projectile_buffer, i);
                    break;
                }
            } if (j != WB_ENEMY_CNT_MAX) continue;
            // powerup hit?
            for (j = 0; j < WB_PARTICLE_CNT_MAX; j++) {
                if (particles[j].head.type == WB_PARTICLE_POWERUP &&
                    projectile->head.pos.x > particles[j].head.pos.x - WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.x <= particles[j].head.pos.x + WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 &&
                    projectile->head.pos.y > particles[j].head.pos.y - WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 && projectile->head.pos.y <= particles[j].head.pos.y + WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2) {
                    
                    wbBufferRemove(projectile_buffer, i);
                    break;
                }
            } if (j != WB_PARTICLE_CNT_MAX) continue;
            // map hit?
            if (projectile->head.pos.x < view->center_x - WB_GRAPHIC_MAP_VIEW_WIDTH / 2 || projectile->head.pos.x >= view->center_x + WB_GRAPHIC_MAP_VIEW_WIDTH / 2 ||
                projectile->head.pos.y < 0 || projectile->head.pos.y >= WB_GAMERULE_MAP_HORIZON_HEIGHT ||
                wbMapGetCollision(map, roundf(projectile->head.pos.x), roundf(projectile->head.pos.y), gamestate->level)) {
                
                wbBufferRemove(projectile_buffer, i);
                continue;
            }
            view->bullet_wiz_cnt += projectile->head.type == WB_PROJECTILE_BULLET_WIZ
                                 || projectile->head.type == WB_PROJECTILE_BLAZER_WIZ;
            view->bullet_cat_cnt += projectile->head.type == WB_PROJECTILE_BULLET_CAT
                                 || projectile->head.type == WB_PROJECTILE_BLAZER_CAT;
            view->spray           = projectile->head.type == WB_PROJECTILE_SPRAY_NW
                                 || projectile->head.type == WB_PROJECTILE_SPRAY_N
                                 || projectile->head.type == WB_PROJECTILE_SPRAY_NE;
        }
    }
}