#include "wizball.h"

void wbParticleUpdate(WBBufferParticle* particle_buffer, WBWiz* wiz, int* powerup_slot) {
    WBParticle* particle;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &particle_buffer->entries[i];
        if (particle->head.type == WB_PARTICLE_NONE) continue;

        switch ((WBParticleType)particle->head.type) {
            case WB_PARTICLE_POWERUP:
            if (
                particle->head.pos_x > wiz->pos_x - WB_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos_x <= wiz->pos_x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                particle->head.pos_y > wiz->pos_y - WB_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos_y <= wiz->pos_y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(particle_buffer, i);
                *powerup_slot = (*powerup_slot + 1) % WB_POWERUP_SLOT_CNT;
            }
            break;

            case WB_PARTICLE_DECAY:
            // for decay of other enemies with no animated color: set color_key > WB_ENEMY_ANIMATION_COLOR_CNT + 1, dont update anymore
            particle->frame_age++;
            if (particle->head.color_key < WB_PARTICLE_DECAY_ANIMATION_SPEED + 1) {
                particle->head.color_key += WB_ENEMY_ANIMATION_COLOR_SPEED;
                particle->head.color_key -= particle->head.color_key >= WB_ENEMY_ANIMATION_COLOR_CNT ? WB_ENEMY_ANIMATION_COLOR_CNT : 0;
            }
            if ((uint64_t)((double)particle->frame_age * WB_PARTICLE_DECAY_ANIMATION_SPEED) >= WB_PARTICLE_DECAY_ANIMATION_FRAME_CNT) {
                wbBufferRemove(particle_buffer, i);
            }
            
        }

    }

}