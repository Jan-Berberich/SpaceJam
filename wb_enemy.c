#include "wizball.h"

// TODO: add decay particle

void wbEnemyAppend(WBEnemy* enemies, int* enemy_cnt, WBEnemyType type, float pos_x, float pos_y) {
    assert(*enemy_cnt - 1 < WB_ENEMY_CNT_MAX);

    while (enemies->type != WB_ENEMY_NONE) {
        enemies++;
    }

    WBEnemy* enemy = enemies;
    enemy->head.pos_x = pos_x;
    enemy->head.pos_y = pos_y;
    enemy->type = type;
    enemy->animation_frame = -0.5f;
    (*enemy_cnt)++;
    // TODO: movement pattern
}

void wbEnemyRemove(WBEnemy* enemies, int* enemy_cnt, int idx, WBParticle* particles, int* particle_cnt) {
    if (enemies[idx].type == WB_ENEMY_SPINNERBLUE || randfin(time(NULL), 0.0f, 1.0f) < WB_PARTICLE_POWERUP_DROP_CHANCE) {
        wbParticleAppend(particles, particle_cnt, WB_PARTICLE_POWERUP, enemies[idx].head.pos_x, enemies[idx].head.pos_y);
    }
    //wbParticleAppend(particles, particle_cnt, WB_PARTICLE_DECAY, enemies[j].head.pos_x, enemies[j].head.pos_y);
     enemies[idx].type = WB_ENEMY_NONE;
    (*enemy_cnt)--;
}

void wbEnemyUpdate(WBEnemy* enemies, int* enemy_cnt, WBPlayerWiz* wiz, WBParticle* particles, int* particle_cnt) {
    WBEnemy* enemy;
    float animation_speed;
    int animation_frame_cnt;
    for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
        enemy = &enemies[i];
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
            enemy->head.pos_x > wiz->head.pos_x - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos_x <= wiz->head.pos_x + WB_ENEMY_HITBOX_SIZE / 2 &&
            enemy->head.pos_y > wiz->head.pos_y - WB_ENEMY_HITBOX_SIZE / 2 && enemy->head.pos_y <= wiz->head.pos_y + WB_ENEMY_HITBOX_SIZE / 2
        ) {
            wbEnemyRemove(enemies, enemy_cnt, i, particles, particle_cnt);
            wiz->head.health--;
        }

    }

}