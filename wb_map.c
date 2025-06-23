#include "wizball.h"

bool wbMapInit(WBMap* map) {
    WBMapAtlas* map_atlas = &map->atlas;
    int width, height, channel_cnt;
    uint8_t* data = stbi_load(WB_MAP_BACKGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->background, data, width, height);
    stbi_image_free(data);

    data = stbi_load(WB_MAP_COLLIDER_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map collider atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->collider_texture, data, width, height);
    map_atlas->collider = malloc(width * height * (sizeof *map_atlas->collider));
    if (!map_atlas->collider) {
        fprintf(stderr, "Failed to allocate memory for collider texture\n");
        stbi_image_free(data);
        return false;
    }
    for (int i = 0; i < width * height; i++) {
        map_atlas->collider[i] = !!data[i * 4 + 3]; // Alpha channel
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
    map_atlas->dust.width = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT;
    map_atlas->dust.height = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE;
    wbTextureInit(&map_atlas->dust, data, map_atlas->dust.width, map_atlas->dust.height);
    free(data);
    
    return true;
}

bool wbMapGetCollision(WBMap* map, int x, int y, int level) {
    return map->atlas.collider[(level * map->atlas.collider_texture.height / WB_MAP_CNT + y) * map->atlas.collider_texture.width + x];
}