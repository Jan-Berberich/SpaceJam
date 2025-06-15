#include "wizball.h"

void wbEnemyAppend(WBEnemyBuffer* enemy_buffer, WBEnemyType enemy_type, WBVec2f* pos, WBVec2f* vel, WBMovepatternType movepattern_type) {
    WBEnemy* enemy = wbBufferAppend(enemy_buffer, enemy_type, pos);
    enemy->vel.x = vel->x;
    enemy->vel.y = vel->y;
    enemy->movepattern_type = movepattern_type;
}

void wbEnemyRemove(WBEnemyBuffer* enemy_buffer, int idx, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemies = enemy_buffer->entries;
    if ((WBEnemyType)enemies[idx].head.type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_PARTICLE_POWERUP_DROP_CHANCE) {
        wbBufferAppend(&particle_buffer->head, WB_PARTICLE_POWERUP, &enemies[idx].head.pos);
        ma_sound_stop(&sound->fire);
        ma_sound_seek_to_pcm_frame(&sound->powerup_drop, 0);
        ma_sound_start(&sound->powerup_drop);
    }
    else {
        WBParticle* particle = wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, &enemies[idx].head.pos);
        particle->head.color_key = enemies[idx].head.color_key;
        ma_sound_seek_to_pcm_frame(&sound->decay, 0);
        ma_sound_start(&sound->decay);
    }
    wbBufferRemove(enemy_buffer, idx);
    gamestate->score += WB_SCORE_ENEMY;
}

void wbEnemyUpdate(WBEnemyBuffer* enemy_buffer, WBWiz* wiz, WBCat* cat, WBParticleBuffer* particle_buffer, WBGamestate* gamestate, WBSound* sound) {
    WBEnemy* enemy;
    gamestate->enemy_cnt = 0;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_BLINKER:
            gamestate->enemy_cnt--;
            default:
            gamestate->enemy_cnt++;
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
                case WB_MOVEPATTERN_JAY:
                break;
                case WB_MOVEPATTERN_RAID:
                break;
            }
            break;
        }
        enemy->head.color_key += WB_ENEMY_ANIMATION_COLOR_SPEED;
        enemy->head.color_key -= enemy->head.color_key >= WB_ENEMY_ANIMATION_COLOR_CNT ? WB_ENEMY_ANIMATION_COLOR_CNT : 0;
        enemy->head.pos.x += enemy->vel.x;
        enemy->head.pos.y += enemy->vel.y;
        enemy->frame_age++;

        if (enemy->head.pos.x > wiz->pos.x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= wiz->pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > wiz->pos.y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= wiz->pos.y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            wiz->health--;
        }

        if (enemy->head.pos.x > cat->pos.x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= cat->pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > cat->pos.y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= cat->pos.y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate, sound);
            cat->health--;
        }
    }

}