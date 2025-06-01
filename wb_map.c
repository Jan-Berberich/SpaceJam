#include "wb.h"

bool wbMapInit(WBMap* map) {
    WBMapAtlas* map_atlas = &map->atlas;
    int width, height, channel_cnt;
    uint8_t* data = stbi_load("sprite/map_background_atlas.png", &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->background, data, width, height);
    stbi_image_free(data);

    data = stbi_load("sprite/map_foreground_atlas.png", &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map foreground atlas image\n");
        return false;
    }
    wbTextureInit(&map_atlas->foreground, data, width, height);
    map_atlas->collider = malloc(width * height * (sizeof *map_atlas->collider));
    if (!map_atlas->collider) {
        fprintf(stderr, "Failed to allocate memory for foreground collider\n");
        stbi_image_free(data);
        return false;
    }
    for (int i = 0; i < width * height; ++i) {
        map_atlas->collider[i] = !!data[i * 4 + 3]; // Alpha channel
    }
    stbi_image_free(data);

    map->atlas_y = 0;
    map->type = WB_MAP_0;
    map->collider = map_atlas->collider;

    return true;
}