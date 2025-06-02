#include "wizball.h"

// TODO: add decay particle

void wbEnemyRemove(WBBufferEnemy* enemy_buffer, int idx, WBBufferParticle* particle_buffer) {
    WBEnemy* enemies = enemy_buffer->entries;
    if (enemies[idx].type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_PARTICLE_POWERUP_DROP_CHANCE) {
        wbBufferAppend(&particle_buffer->head, WB_PARTICLE_POWERUP, enemies[idx].pos_x, enemies[idx].pos_y);
    }
    //wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, enemies[j].head.pos_x, enemies[j].head.pos_y);
    wbBufferRemove(&enemy_buffer->head, idx);
}

void wbEnemyUpdate(WBBufferEnemy* enemy_buffer, WBWiz* wiz, WBBufferParticle* particle_buffer) {
    WBEnemy* enemy;
    float animation_speed;
    int animation_frame_cnt;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch (enemy->type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERBLUE:
            animation_speed = WB_ENEMY_SPINNERBLUE_ANIMATION_SPEED;
            animation_frame_cnt = WB_ENEMY_SPINNERBLUE_ANIMATION_FRAME_CNT;
            break;
        }
        enemy->animation_frame += animation_speed;
        enemy->animation_frame -= enemy->animation_frame >= -0.5f + animation_frame_cnt ? animation_frame_cnt : 0;

        if (
            enemy->pos_x > wiz->pos_x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->pos_x <= wiz->pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->pos_y > wiz->pos_y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->pos_y <= wiz->pos_y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer);
            wiz->health--;
        }

    }

}