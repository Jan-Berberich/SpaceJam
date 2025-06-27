#include "wizball.h"

void wbParticleUpdate(WBParticleBuffer* particle_buffer, WBPlayer* player, WBGamestate* gamestate, WBSound* sound) {
    WBParticle* particle;
    for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
        particle = &particle_buffer->entries[i];
        if (particle->head.type == WB_PARTICLE_NONE) continue;

        switch (particle->head.type) {
            case WB_PARTICLE_POWERUP:
            if (gamestate->state == WB_GAMESTATE_PLAY &&
                particle->head.pos.x > player->wiz.pos.x - WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos.x <= player->wiz.pos.x + WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 &&
                particle->head.pos.y > player->wiz.pos.y - WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2 && particle->head.pos.y <= player->wiz.pos.y + WB_GAMERULE_PARTICLE_HITBOX_SIZE / 2) {
                
                wbBufferRemove(particle_buffer, i);
                ma_sound_seek_to_pcm_frame(&sound->powerup_collect, 0);
                ma_sound_start(&sound->powerup_collect);
                gamestate->powerup.slot = (gamestate->powerup.slot + 1) % WB_POWERUP_SLOT_CNT;
                gamestate->score += WB_GAMERULE_SCORE_POWERUP;
            }
            break;

            case WB_PARTICLE_DECAY:
            // for decay of other enemies with no animated color: set color_key > WB_ENEMY_COLORMAP_CNT + 1, dont update anymore
            if (particle->head.color_key >= 0) {
                particle->head.color_key += WB_GRAPHIC_ENEMY_COLORMAP_SPEED * gamestate->delta_time;
                particle->head.color_key -= particle->head.color_key >= WB_GRAPHIC_ENEMY_COLORMAP_CNT ? WB_GRAPHIC_ENEMY_COLORMAP_CNT : 0;
            }
            particle->head.animation_key += WB_GRAPHIC_PARTICLE_ANIMATION_SPEED * gamestate->delta_time;
            if (particle->head.animation_key >= WB_GRAPHIC_PARTICLE_ANIMATION_FRAME_CNT) {
                wbBufferRemove(particle_buffer, i);
            }
            break;

            case WB_PARTICLE_DROPLET_FALL:
            particle->head.pos.y += WB_GAMERULE_PARTICLE_DROPLET_FALL_VEL * gamestate->delta_time;
            particle->head.animation_key += WB_GRAPHIC_PARTICLE_ANIMATION_SPEED * gamestate->delta_time;
            particle->head.animation_key -= particle->head.animation_key >= WB_GRAPHIC_PARTICLE_ANIMATION_FRAME_CNT ? WB_GRAPHIC_PARTICLE_ANIMATION_FRAME_CNT : 0;
            if (particle->head.pos.y >= WB_GAMERULE_MAP_FLOOR_HEIGHT) {
                int idx = wbBufferAppend(particle_buffer, WB_PARTICLE_DROPLET_SPLAT, &particle->head.pos);
                particle_buffer->entries[idx].head.color_key = particle->head.color_key;
                wbBufferRemove(particle_buffer, i);
            }
            break;

            case WB_PARTICLE_DROPLET_SPLAT:
            particle->head.animation_key += WB_GRAPHIC_PARTICLE_ANIMATION_SPEED * gamestate->delta_time;
            if (particle->head.animation_key >= WB_GRAPHIC_PARTICLE_DROPLET_SPLAT_ANIMATION_CNT) {
                wbBufferRemove(particle_buffer, i);
            }
            break;

            
        }

    }

}