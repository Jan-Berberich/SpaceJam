#include "wizball.h"

void wbGamestateSetupTitlescreen(WBGamestate* gamestate, WBSound* sound) {
    if (gamestate->state != WB_GAMESTATE_HIGHSCORE) {
        ma_sound_seek_to_pcm_frame(&sound->titlescreen, 0);
        ma_sound_start(&sound->titlescreen);
    }
    gamestate->lifes = 3;
    gamestate->score = 0;
    gamestate->highscore = 50000;
    gamestate->score2 = 0;
    gamestate->level = 0;
    gamestate->powerup.unlocked = WB_POWERUP_NONE;
    gamestate->powerup.permanent = WB_POWERUP_NONE;
    gamestate->frame_counter = 0;
    gamestate->state = WB_GAMESTATE_TITLESCREEN;
}

void wbGamestateSetupGetready(WBGamestate* gamestate, WBSound* sound, WBMap* map,
    WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBProjectileBuffer* projectile_buffer) {
    
    ma_sound_stop(&sound->titlescreen);
    ma_sound_seek_to_pcm_frame(&sound->getready, 0);
    ma_sound_start(&sound->getready);
    wbBufferClear(particle_buffer);
    wbBufferClear(enemy_buffer);
    wbBufferClear(projectile_buffer);
    map->view.center_x = 0.0f;
    for (int i = 0; i < 4; i++) {
        wbEnemyInsertRandoms(enemy_buffer, gamestate->frame_counter);
    }
    gamestate->frame_counter = 0;
    gamestate->state = WB_GAMESTATE_GETREADY;
}