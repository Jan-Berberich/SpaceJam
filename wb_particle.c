#include "wizball.h"

void wbParticleUpdate(WBParticleBuffer* particle_buffer, WBWiz* wiz, WBGamestate* gamestate, WBSound* sound) {
    WBParticle* particle;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &particle_buffer->entries[i];
        if (particle->head.type == WB_PARTICLE_NONE) continue;

        switch (particle->head.type) {
            case WB_PARTICLE_POWERUP:
            if (
                particle->head.pos.x > wiz->pos.x - WB_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos.x <= wiz->pos.x + WB_PARTICLE_HITBOX_SIZE / 2 &&
                particle->head.pos.y > wiz->pos.y - WB_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos.y <= wiz->pos.y + WB_PARTICLE_HITBOX_SIZE / 2
            ) {
                wbBufferRemove(particle_buffer, i);
                ma_sound_seek_to_pcm_frame(&sound->powerup_collect, 0);
                ma_sound_start(&sound->powerup_collect);
                gamestate->powerup.slot = (gamestate->powerup.slot + 1) % WB_POWERUP_SLOT_CNT;
                gamestate->score += WB_SCORE_POWERUP;
            }
            break;

            case WB_PARTICLE_DECAY:
            // for decay of other enemies with no animated color: set color_key > WB_ENEMY_ANIMATION_COLOR_CNT + 1, dont update anymore
            particle->head.color_key += WB_ENEMY_ANIMATION_COLOR_SPEED;
            particle->head.color_key -= particle->head.color_key >= WB_ENEMY_ANIMATION_COLOR_CNT ? WB_ENEMY_ANIMATION_COLOR_CNT : 0;
            particle->frame_age++;
            if ((uint64_t)((double)particle->frame_age * WB_PARTICLE_DECAY_ANIMATION_SPEED) >= WB_PARTICLE_DECAY_ANIMATION_FRAME_CNT) {
                wbBufferRemove(particle_buffer, i);
            }
            
        }

    }

}