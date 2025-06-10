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
    
    return true;
}

bool wbMapGetCollision(WBMap* map, int x, int y, int level) {
    return map->atlas.collider[(level * map->atlas.collider_texture.height / WB_MAP_CNT + y) * map->atlas.collider_texture.width + x];
}