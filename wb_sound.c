#include "wizball.h"

bool wbSoundInit(WBSound* sound) {
    if (ma_engine_init(NULL, &sound->_engine) != MA_SUCCESS)
        {fprintf(stderr, "Failed to initialize sound engine\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_TITLESCREEN_PATH, MA_SOUND_FLAG_LOOPING, NULL, NULL, &sound->titlescreen) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load titlescreen sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_GETREADY_PATH, 0, NULL, NULL, &sound->getready) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load getready sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_FIRE_PATH, 0, NULL, NULL, &sound->fire) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load fire sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_FIRE_SPAM_PATH, 0, NULL, NULL, &sound->fire_spam) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load fire spam sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_POWERUP_DROP_PATH, 0, NULL, NULL, &sound->powerup_drop) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load powerup drop sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_POWERUP_COLLECT_PATH, 0, NULL, NULL, &sound->powerup_collect) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load powerup collect sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_POWERUP_ACTIVATE_PATH, 0, NULL, NULL, &sound->powerup_activate) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load powerup activate sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_DECAY_PATH, 0, NULL, NULL, &sound->decay) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load decay sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_CLEAR_PATH, 0, NULL, NULL, &sound->clear) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load clear sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_BLINKER_PATH, 0, NULL, NULL, &sound->blinker) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load blinker sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_BOMB_PATH, 0, NULL, NULL, &sound->bomb) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load bomb sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_CATHIT_PATH, 0, NULL, NULL, &sound->cathit) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load cathit sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_WIZDEATH_PATH, 0, NULL, NULL, &sound->wizdeath) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load wizdeath sound\n"); wbSoundUninit(sound); return false;}
    if (ma_sound_init_from_file(&sound->_engine, WB_SOUND_GAMEOVER_PATH, 0, NULL, NULL, &sound->gameover) != MA_SUCCESS)
        {fprintf(stderr, "Failed to load gameover sound\n"); wbSoundUninit(sound); return false;}
    
    return true;
}

void wbSoundUninit(WBSound* sound) {
    if (&sound->gameover) ma_sound_uninit(&sound->gameover);
    if (&sound->wizdeath) ma_sound_uninit(&sound->wizdeath);
    if (&sound->cathit) ma_sound_uninit(&sound->cathit);
    if (&sound->bomb) ma_sound_uninit(&sound->bomb);
    if (&sound->blinker) ma_sound_uninit(&sound->blinker);
    if (&sound->clear) ma_sound_uninit(&sound->clear);
    if (&sound->decay) ma_sound_uninit(&sound->decay);
    if (&sound->powerup_activate) ma_sound_uninit(&sound->powerup_activate);
    if (&sound->powerup_collect) ma_sound_uninit(&sound->powerup_collect);
    if (&sound->powerup_drop) ma_sound_uninit(&sound->powerup_drop);
    if (&sound->fire_spam) ma_sound_uninit(&sound->fire_spam);
    if (&sound->fire) ma_sound_uninit(&sound->fire);
    if (&sound->getready) ma_sound_uninit(&sound->getready);
    if (&sound->titlescreen) ma_sound_uninit(&sound->titlescreen);
    if (&sound->_engine) ma_engine_uninit(&sound->_engine);
}