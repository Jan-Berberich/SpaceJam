#include "wizball.h"

void wbParticleAppend(WBParticle* particles, int* particle_cnt, WBParticleType type, float pos_x, float pos_y) {
    assert(*particle_cnt - 1 < WB_PARTICLE_CNT_MAX);

    while (particles->type != WB_PARTICLE_NONE) {
        particles++;
    }

    WBParticle* particle = particles;
    particle->pos_x = pos_x;
    particle->pos_y = pos_y;
    particle->type = type;
}

void wbParticleRemove(WBParticle* particles, int* particle_cnt, int idx) {
     particles[idx].type = WB_PARTICLE_NONE;
    (*particle_cnt)--;
}

void wbParticleUpdate(WBParticle* particles, int* particle_cnt, WBPlayerWiz* wiz, int* powerup_slot, unsigned long long frame_cnt) {
    WBParticle* particle;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &particles[i];
        if (particle->type == WB_PARTICLE_NONE) continue;

        switch (particle->type) {
            case WB_PARTICLE_POWERUP:
            if (
                particle->pos_x > wiz->head.pos_x - WB_HITBOX_SIZE / 2 && particle->pos_x <= wiz->head.pos_x + WB_HITBOX_SIZE / 2 &&
                particle->pos_y > wiz->head.pos_y - WB_HITBOX_SIZE / 2 && particle->pos_y <= wiz->head.pos_y + WB_HITBOX_SIZE / 2
            ) {
                wbParticleRemove(particles, particle_cnt, i);
                *powerup_slot = (*powerup_slot + 1) % WB_POWERUP_SLOT_CNT;
            }
            break;

            /*
            case X:
            if (frame_cnt - particle->creation_frame >= X_LIFETIME_FRAME_CNT) {
                wbParticleRemove(particles, particle_cnt, i);
            }
            */

        }

    }

}