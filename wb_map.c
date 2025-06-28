#include "wizball.h"

bool wbMapInitCollider(WBMap* map, uint8_t* data) {
    WBTexture* map_atlas = &map->graphic_handle->foreground_atlas;
    map->collider_atlas = malloc(map_atlas->width * map_atlas->height * (sizeof *map->collider_atlas));
    if (!map->collider_atlas) {
        fprintf(stderr, "Failed to allocate memory for map collider_atlas atlas\n");
        return false;
    }
    for (int i = 0; i < map_atlas->width * map_atlas->height; i++) {
        map->collider_atlas[i] = !!data[i * 4 + 3]; // Alpha channel
    }
    return true;
}

bool wbMapGetCollision(WBMap* map, int x, int y, int level) {
    WBTexture* map_atlas = &map->graphic_handle->foreground_atlas;
    return map->collider_atlas[(int)((level * map_atlas->height / WB_MAP_CNT + y) * map_atlas->width + x + 0.5f)];
}