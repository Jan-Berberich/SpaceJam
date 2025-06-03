#include "wizball.h"

void wbEnemyRemove(WBBufferEnemy* enemy_buffer, int idx, WBBufferParticle* particle_buffer) {
    WBEnemy* enemies = enemy_buffer->entries;
    if (enemies[idx].type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_PARTICLE_POWERUP_DROP_CHANCE) {
        wbBufferAppend(&particle_buffer->head, WB_PARTICLE_POWERUP, enemies[idx].pos_x, enemies[idx].pos_y);
    }
    else {
        wbBufferAppend(particle_buffer, WB_PARTICLE_DECAY, enemies[idx].pos_x, enemies[idx].pos_y);
    }
    wbBufferRemove(enemy_buffer, idx);
}

void wbEnemyUpdate(WBBufferEnemy* enemy_buffer, WBWiz* wiz, WBBufferParticle* particle_buffer) {
    WBEnemy* enemy;
    double time = glfwGetTime();
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemy_buffer->entries[i];
        switch (enemy->type) {
            case WB_ENEMY_NONE:
            continue;

            case WB_ENEMY_SPINNERBLUE:
            break;

            case WB_ENEMY_CIRCLE:
            // TODO: for debug
            enemy->pos_x += 50.0f * cosf(M_2PI / 4 * enemy->frame_age / WB_FPS) / WB_FPS;
            enemy->pos_y += 50.0f * sinf(M_2PI / 4 * enemy->frame_age / WB_FPS) / WB_FPS;
            break;
        }
        enemy->frame_age++;

        if (
            enemy->pos_x > wiz->pos_x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->pos_x <= wiz->pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->pos_y > wiz->pos_y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->pos_y <= wiz->pos_y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemy_buffer, i, particle_buffer);
            wiz->health--;
        }

    }

}