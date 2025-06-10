#include "wizball.h"

void wbEnemyRemove(WBBufferEnemy* enemy_buffer, int idx, WBBufferParticle* particle_buffer, WBGamestate* gamestate) {
    WBEnemy* enemies = enemy_buffer->entries;
    if ((WBEnemyType)enemies[idx].head.type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_PARTICLE_POWERUP_DROP_CHANCE) {
        wbBufferAppend(&particle_buffer->head, WB_PARTICLE_POWERUP, &enemies[idx].head.pos);
    }
    else {
        WBParticle* particle = wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, &enemies[idx].head.pos);
        particle->head.color_key = enemies[idx].head.color_key;
    }
    wbBufferRemove(enemy_buffer, idx);
    gamestate->score += WB_SCORE_ENEMY;
}

void wbEnemyUpdate(WBBufferEnemy* enemy_buffer, WBWiz* wiz, WBCat* cat, WBBufferParticle* particle_buffer, WBGamestate* gamestate) {
    WBEnemy* enemy;
    gamestate->enemy_cnt = WB_ENEMY_CNT_MAX;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch ((WBEnemyType)enemy->head.type) {
            case WB_ENEMY_NONE:
            gamestate->enemy_cnt--;
            continue;

            case WB_ENEMY_SPINNERBLUE:
            break;

            case WB_ENEMY_CIRCLE:
            // TODO: for debug
            enemy->head.pos.x += 50.0f * cosf(M_2PI / 4 * enemy->frame_age / WB_FPS) / WB_FPS;
            enemy->head.pos.y += 50.0f * sinf(M_2PI / 4 * enemy->frame_age / WB_FPS) / WB_FPS;
            break;
        }
        enemy->head.color_key += WB_ENEMY_ANIMATION_COLOR_SPEED;
        enemy->head.color_key -= enemy->head.color_key >= WB_ENEMY_ANIMATION_COLOR_CNT ? WB_ENEMY_ANIMATION_COLOR_CNT : 0;
        enemy->frame_age++;

        if (enemy->head.pos.x > wiz->pos.x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= wiz->pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > wiz->pos.y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= wiz->pos.y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate);
            wiz->health--;
        }

        if (enemy->head.pos.x > cat->pos.x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.x <= cat->pos.x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos.y > cat->pos.y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos.y <= cat->pos.y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer, gamestate);
            cat->health--;
        }
    }

}