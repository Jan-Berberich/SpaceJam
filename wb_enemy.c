#include "wizball.h"

int wbEnemyAppend(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_type, int colorpallet_offset, WBVec2f* pos, WBVec2f* vel, WBMovepatternType movepattern_type) {
    int idx = wbBufferAppend(enemy_buffer, enemy_type, pos);
    WBEnemy* enemy = &enemy_buffer->entries[idx];
    enemy->vel.x = vel->x;
    enemy->vel.y = vel->y;
    enemy->movepattern_type = movepattern_type;
    enemy->time = 0.0;
    enemy->head.color_key = colorpallet_offset == WB_GRAPHIC_ENEMY_COLORMAP_OFFSET ?
        randfin(glfwGetTime() * 1e9, 0.0f, WB_GRAPHIC_ENEMY_COLORMAP_CNT) :
        colorpallet_offset - WB_GRAPHIC_ENEMY_COLORMAP_OFFSET;
    if (enemy->head.type == WB_ENEMY_SPINNERCYAN || enemy->head.type == WB_ENEMY_SPINNERYELLOW) {
        enemy->head.animation_key = randfin(glfwGetTime() * 1e9, 0.0, WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_FRAME_CNT);
    }
    return idx;
}

void wbEnemyPopulate(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_tpye, int colorpallet_offset, WBMovepatternType movepattern_type, WBView* view) {
    WBVec2f pos;
    WBVec2f vel = {0.0f};
    uint32_t seed = glfwGetTime() * 1e9;
    switch (movepattern_type) {
        case WB_MOVEPATTERN_INERT:
        for (int i = 0; i < 8; i++) {
            pos.x = randfin(seed++, 200, 3000);
            if (pos.x > view->center_x - WB_GRAPHIC_WINDOW_WIDTH / 2 && pos.x < view->center_x + WB_GRAPHIC_WINDOW_WIDTH / 2) {
                i--;
                continue;
            }
            pos.y = randfin(seed++,  50,  200);
            wbEnemyAppend(enemy_buffer, enemy_tpye, colorpallet_offset, &pos, &vel, movepattern_type);
        }
        break;
        case WB_MOVEPATTERN_CIRCLE:
        for (int i = 0; i < 8; i++) { // TODO: temporary
            pos.x = randfin(seed++, 200, 3000);
            if (pos.x > view->center_x - WB_GRAPHIC_WINDOW_WIDTH / 2 && pos.x < view->center_x + WB_GRAPHIC_WINDOW_WIDTH / 2) {
                i--;
                continue;
            }
            pos.y = randfin(seed++,   0,  150);
            wbEnemyAppend(enemy_buffer, enemy_tpye, colorpallet_offset, &pos, &vel, movepattern_type);
        }
    }
}

void wbEnemyInsertRandoms(WBEnemyBuffer* enemy_buffer, double time) {
    uint32_t seed = glfwGetTime() * 1e9;
    WBVec2f pos;
    float pos_y_offset = WB_GRAPHIC_TEXT_OFFSET_Y + 0.5 * WB_GRAPHIC_TEXT_LARGE_SPRITE_SIZE;
    pos.x = randfin(seed++,                          0.0f, 0.5f * WB_GRAPHIC_WINDOW_WIDTH);
    pos.y = randfin(seed++, 0.5f * WB_GRAPHIC_SPRITE_SIZE, 0.5f * WB_GRAPHIC_WINDOW_HEIGHT / 2) + 0.5f * pos_y_offset;
    WBVec2f vel;
    vel.x = randfin(seed++, 0.25f * WB_GAMERULE_PLAYER_WIZ_VEL_X_6, 2.0f * WB_GAMERULE_PLAYER_WIZ_VEL_X_6);
    vel.y = randfin(seed++,                                   0.0f, 2.0f * WB_GAMERULE_PLAYER_WIZ_VEL_X_6);
    vel.x *= (*((uint8_t*)&vel.x) & 1) * 2 - 1;
    for (int i = 0; i < 4; i++) {
        pos.x *= -1.0f;
        vel.x *= -1.0f;
        if (i == 2) {
            pos.y = -pos.y + pos_y_offset;
            vel.y = -vel.y;
        }
        int idx = wbEnemyAppend(enemy_buffer, WB_ENEMY_CIRCLE, WB_GRAPHIC_ENEMY_COLORMAP_OFFSET, &pos, &vel, WB_MOVEPATTERN_INERT);
        enemy_buffer->entries[idx].head.color_key = fmod(time * WB_GRAPHIC_ENEMY_COLORMAP_SPEED, WB_GRAPHIC_ENEMY_COLORMAP_CNT);
        wbBufferRemove(enemy_buffer, (idx + 1) % WB_ENEMY_CNT_MAX);
    }
}

void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemies = enemy_buffer->entries;
    int particle_idx;
    switch (enemies[idx].head.type) {
        case WB_ENEMY_SPINNERCYAN:
        wbBufferAppend(particle_buffer, WB_PARTICLE_POWERUP, &enemies[idx].head.pos);
        ma_sound_stop(&sound->fire);
        ma_sound_seek_to_pcm_frame(&sound->powerup_drop, 0);
        ma_sound_start(&sound->powerup_drop);
        break;

        case WB_ENEMY_DROPLET:
        particle_idx = wbBufferAppend(particle_buffer, WB_PARTICLE_DROPLET_FALL, &enemies[idx].head.pos);
        particle_buffer->entries[particle_idx].head.color_key = enemies[idx].head.color_key;
        // TODO: play sound
        break;

        default:
        if (randfin(time(NULL), 0.0f, 1.0f) < WB_GAMERULE_PARTICLE_POWERUP_DROP_CHANCE) {
            wbBufferAppend(particle_buffer, WB_PARTICLE_POWERUP, &enemies[idx].head.pos);
            ma_sound_stop(&sound->fire);
            ma_sound_seek_to_pcm_frame(&sound->powerup_drop, 0);
            ma_sound_start(&sound->powerup_drop);
        } else {
            particle_idx = wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, &enemies[idx].head.pos);
            particle_buffer->entries[particle_idx].head.color_key = enemies[idx].head.color_key;
            ma_sound_seek_to_pcm_frame(&sound->decay, 0);
            ma_sound_start(&sound->decay);
        }
        break;
    }
    wbBufferRemove(enemy_buffer, idx);
    gamestate->score += WB_GAMERULE_SCORE_ENEMY;
}

bool wbEnemyMovepatternUpdate(WBEnemy* enemy) {
    switch (enemy->movepattern_type) {
        case WB_MOVEPATTERN_INERT:
        break;
        case WB_MOVEPATTERN_GLIDE:
        break;
        case WB_MOVEPATTERN_STEP_DOWN:
        break;
        case WB_MOVEPATTERN_STEP_UP:
        break;
        case WB_MOVEPATTERN_CIRCLE:
        enemy->vel.x = 100.0f * cosf(2.0f * enemy->time);
        enemy->vel.y = 100.0f * sinf(2.0f * enemy->time);
        break;
        case WB_MOVEPATTERN_BOUNCE:
        break;
        case WB_MOVEPATTERN_BOUNCE_FLOOR:
        break;
        case WB_MOVEPATTERN_BOUNCE_CEIL:
        break;
        case WB_MOVEPATTERN_ARC:
        break;
        case WB_MOVEPATTERN_RAID:
        break;
    }
}

void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBMap* map, WBPlayer* player, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemy;
    gamestate->enemy_cnt = WB_ENEMY_CNT_MAX;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch (enemy->head.type) {
            case WB_ENEMY_NONE:
            gamestate->enemy_cnt--;
            continue;

            case WB_ENEMY_BLINKER:
            gamestate->enemy_cnt--;
            wbEnemyMovepatternUpdate(enemy);
            break;

            case WB_ENEMY_SPINNERCYAN:
            wbEnemyMovepatternUpdate(enemy);
            enemy->head.animation_key += WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_SPEED * gamestate->delta_time;
            enemy->head.animation_key -= enemy->head.animation_key >= WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_FRAME_CNT ? WB_GRAPHIC_ENEMY_SPINNER_ANIMATION_FRAME_CNT : 0;
            break;

            case WB_ENEMY_CIRCLE:
            wbEnemyMovepatternUpdate(enemy);
            break;

            case WB_ENEMY_DROPLET:
            wbEnemyMovepatternUpdate(enemy);
            if (enemy->head.pos.y < WB_GAMERULE_MAP_CEIL_HEIGHT || enemy->head.pos.y > WB_GAMERULE_MAP_FLOOR_HEIGHT ||
                wbMapGetCollision(map, enemy->head.pos.x, enemy->head.pos.y, gamestate->level)) {
            
                enemy->head.animation_key += WB_GRAPHIC_ENEMY_DROPLET_ANIMATION_SPEED * gamestate->delta_time;
            } else {
                enemy->head.animation_key += enemy->head.animation_key > 0 ? WB_GRAPHIC_ENEMY_DROPLET_ANIMATION_SPEED * gamestate->delta_time : 0;
            }
            enemy->head.animation_key = enemy->head.animation_key >= WB_GRAPHIC_ENEMY_DROPLET_ANIMATION_FRAME_CNT ? 0 : enemy->head.animation_key;
            break;
        }

        enemy->head.pos.x += enemy->vel.x * gamestate->delta_time;
        enemy->head.pos.y += enemy->vel.y * gamestate->delta_time;
        if (enemy->head.color_key >= 0) {
            enemy->head.color_key += WB_GRAPHIC_ENEMY_COLORMAP_SPEED * gamestate->delta_time;
            enemy->head.color_key -= enemy->head.color_key >= WB_GRAPHIC_ENEMY_COLORMAP_CNT ? WB_GRAPHIC_ENEMY_COLORMAP_CNT : 0;
        }
        enemy->time += gamestate->delta_time;

        if (gamestate->state == WB_GAMESTATE_PLAY &&
            enemy->head.pos.x > player->cat.pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= player->cat.pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > player->cat.pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= player->cat.pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2) {
            
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            if (--player->cat.health) {
                ma_sound_seek_to_pcm_frame(&sound->cathit, 0);
                ma_sound_start(&sound->cathit);
            }
            continue;
        }

        if (gamestate->state == WB_GAMESTATE_PLAY &&
            enemy->head.pos.x > player->wiz.pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= player->wiz.pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > player->wiz.pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= player->wiz.pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2) {
            
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            player->wiz.health--;
        }
    }
}