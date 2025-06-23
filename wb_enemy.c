#include "wizball.h"

int wbEnemyAppend(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_type, WBVec2f* pos, WBVec2f* vel, WBMovepatternType movepattern_type) {
    int idx = wbBufferAppend(enemy_buffer, enemy_type, pos);
    WBEnemy* enemy = &enemy_buffer->entries[idx];
    enemy->vel.x = vel->x;
    enemy->vel.y = vel->y;
    enemy->movepattern_type = movepattern_type;
    enemy->frame_age = 0;
    return idx;
}

void wbEnemyInsertRandoms(WBEnemyBuffer* enemy_buffer, uint64_t frame_counter) {
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
        int idx = wbEnemyAppend(enemy_buffer, WB_ENEMY_CIRCLE, &pos, &vel, WB_MOVEPATTERN_INERT);
        enemy_buffer->entries[idx].head.color_key = fmod(frame_counter * WB_GRAPHIC_ENEMY_ANIMATION_COLOR_SPEED, WB_GRAPHIC_ENEMY_ANIMATION_COLOR_CNT);
        wbBufferRemove(enemy_buffer, (idx + 1) % WB_ENEMY_CNT_MAX);
    }
}

void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemies = enemy_buffer->entries;
    if ((WBEnemyType)enemies[idx].head.type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_GAMERULE_PARTICLE_POWERUP_DROP_CHANCE) {
        wbBufferAppend(&particle_buffer->head, WB_PARTICLE_POWERUP, &enemies[idx].head.pos);
        ma_sound_stop(&sound->fire);
        ma_sound_seek_to_pcm_frame(&sound->powerup_drop, 0);
        ma_sound_start(&sound->powerup_drop);
    }
    else {
        int particle_idx = wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, &enemies[idx].head.pos);
        WBParticle* particle = &particle_buffer->entries[particle_idx];
        particle->head.color_key = enemies[idx].head.color_key;
        ma_sound_seek_to_pcm_frame(&sound->decay, 0);
        ma_sound_start(&sound->decay);
    }
    wbBufferRemove(enemy_buffer, idx);
    gamestate->score += WB_GAMERULE_SCORE_ENEMY;
}

void wbEnemyMovepatternUpdate(WBEnemy* enemy) {
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
        enemy->vel.x = 100.0f * cosf(2.0f * enemy->frame_age / WB_FPS) / WB_FPS;
        enemy->vel.y = 100.0f * sinf(2.0f * enemy->frame_age / WB_FPS) / WB_FPS;
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

void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBWiz* wiz, WBCat* cat, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemy;
    gamestate->enemy_cnt = WB_ENEMY_CNT_MAX;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            gamestate->enemy_cnt--;
            continue;

            case WB_ENEMY_BLINKER:
            gamestate->enemy_cnt--;
            wbEnemyMovepatternUpdate(enemy);
            break;

            case WB_ENEMY_SPINNERBLUE:
            enemy->head.animation_key += WB_GRAPHIC_ENEMY_SPINNERBLUE_ANIMATION_SPEED;
            enemy->head.animation_key -= enemy->head.animation_key >= WB_GRAPHIC_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT ? WB_GRAPHIC_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT : 0;
            wbEnemyMovepatternUpdate(enemy);
            break;

            case WB_ENEMY_CIRCLE:
            wbEnemyMovepatternUpdate(enemy);
            break;
        }

        enemy->head.pos.x += enemy->vel.x;
        enemy->head.pos.y += enemy->vel.y;
        if (enemy->head.color_key >= 0) {
            enemy->head.color_key += WB_GRAPHIC_ENEMY_ANIMATION_COLOR_SPEED;
            enemy->head.color_key -= enemy->head.color_key >= WB_GRAPHIC_ENEMY_ANIMATION_COLOR_CNT ? WB_GRAPHIC_ENEMY_ANIMATION_COLOR_CNT : 0;
        }
        enemy->frame_age++;

        if (gamestate->state == WB_GAMESTATE_PLAY &&
            enemy->head.pos.x > cat->pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= cat->pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > cat->pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= cat->pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2) {
            
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            if (--cat->health) {
                ma_sound_seek_to_pcm_frame(&sound->cathit, 0);
                ma_sound_start(&sound->cathit);
            }
            continue;
        }

        if (gamestate->state == WB_GAMESTATE_PLAY &&
            enemy->head.pos.x > wiz->pos.x - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= wiz->pos.x + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > wiz->pos.y - WB_GAMERULE_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= wiz->pos.y + WB_GAMERULE_ENEMY_HITBOX_SIZE / 2) {
            
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            wiz->health--;
        }
    }
}