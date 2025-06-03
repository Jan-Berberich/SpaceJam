#include "wizball.h"

void wbParticleUpdate(WBBufferParticle* particle_buffer, WBWiz* wiz, int* powerup_slot) {
    WBParticle* particle;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &particle_buffer->entries[i];
        if (particle->type == WB_PARTICLE_NONE) continue;

        switch (particle->type) {
            case WB_PARTICLE_POWERUP:
            if (
                particle->pos_x > wiz->pos_x - WB_PARTICLE_HITBOX_SIZE / 2 && particle->pos_x <= wiz->pos_x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                particle->pos_y > wiz->pos_y - WB_PARTICLE_HITBOX_SIZE / 2 && particle->pos_y <= wiz->pos_y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(particle_buffer, i);
                *powerup_slot = (*powerup_slot + 1) % WB_POWERUP_SLOT_CNT;
            }
            break;

            case WB_PARTICLE_DECAY:
            particle->frame_age++;
            if ((uint64_t)((double)particle->frame_age * WB_PARTICLE_DECAY_ANIMATION_SPEED) >= WB_PARTICLE_DECAY_ANIMATION_FRAME_CNT) {
                wbBufferRemove(particle_buffer, i);
            }
            
        }

    }

}