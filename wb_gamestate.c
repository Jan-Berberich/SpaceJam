#include "wizball.h"

void wbGamestateSetupTitlescreen(WBGamestate* gamestate, WBAudio* audio) {
    if (gamestate->state == WB_GAMESTATE_HIGHSCORE) {} // TODO: play highscore music
    else {
        wbAudioStart(audio, &audio->sound.titlescreen);
    }
    gamestate->lifes = 3;
    gamestate->score = 0;
    gamestate->highscore = 50000;
    gamestate->score2 = 0;
    gamestate->level = 0;
    gamestate->powerup.unlocked = WB_POWERUP_NONE;
    gamestate->powerup.permanent = WB_POWERUP_NONE;
    gamestate->time = gamestate->delta_time;
    gamestate->state = WB_GAMESTATE_TITLESCREEN;
}

void wbGamestateSetupGetready(WBGamestate* gamestate, WBView* view,
    WBEnemyBuffer* enemy_buffer, WBParticleBuffer* particle_buffer, WBProjectileBuffer* projectile_buffer, WBAudio* audio) {
    
    wbAudioStop(audio, &audio->sound.titlescreen);
    wbAudioStart(audio, &audio->sound.getready);

    wbBufferClear(particle_buffer);
    wbBufferClear(enemy_buffer);
    wbBufferClear(projectile_buffer);
    view->center_x = 0.0f;
    gamestate->time = -gamestate->delta_time;
    gamestate->state = WB_GAMESTATE_GETREADY;
}