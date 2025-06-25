#include "wizball.h"

bool wbMapInit(WBMap* map) {
    int width, height, channel_cnt;
    uint8_t* data = stbi_load(WB_MAP_BACKGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        return false;
    }
    wbTextureInit(&map->background_atlas, data, width, height);
    stbi_image_free(data);

    data = stbi_load(WB_MAP_FOREGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map foreground atlas image\n");
        return false;
    }
    wbTextureInit(&map->foreground_atlas, data, width, height);
    map->collider = malloc(width * height * (sizeof *map->collider));
    if (!map->collider) {
        fprintf(stderr, "Failed to allocate memory for map collider\n");
        stbi_image_free(data);
        return false;
    }
    for (int i = 0; i < width * height; i++) {
        map->collider[i] = !!data[i * 4 + 3]; // Alpha channel
    }
    stbi_image_free(data);

    data = malloc(WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT * 4 * (sizeof *data));
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for dust texture\n");
        return false;
    }
    uint8_t* p = data;
    srand(15);
    for (int i = 0; i < WB_GRAPHIC_MAP_DUST_LAYER_CNT; i++) {
        for (int y = 0; y < WB_GRAPHIC_MAP_DUST_SPRITE_SIZE; y++) {
            for (int x = 0; x < WB_GRAPHIC_MAP_DUST_SPRITE_SIZE; x++) {
                // Randomly place dust specks
                uint8_t alpha = (float)rand() / RAND_MAX < WB_GRAPHIC_MAP_DUST_DENSITY ? 128 : 0;
                *p++ = 255; // white dust
                *p++ = 255;
                *p++ = 255;
                *p++ = alpha;
            }
        }
    }
    map->dust_texture.width = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT;
    map->dust_texture.height = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE;
    wbTextureInit(&map->dust_texture, data, map->dust_texture.width, map->dust_texture.height);
    free(data);
    
    return true;
}

bool wbMapGetCollision(WBMap* map, int x, int y, int level) {
    return map->collider[(level * map->foreground_atlas.height / WB_MAP_CNT + y) * map->foreground_atlas.width + x];
}