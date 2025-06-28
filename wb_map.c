#include "wizball.h"

bool wbMapInitCollider(WBMap* map, uint8_t* data) {
    map->collider_atlas = malloc(WB_GRAPHIC_MAP_ATLAS_WIDTH * WB_GRAPHIC_MAP_ATLAS_HEIGHT * (sizeof *map->collider_atlas));
    if (!map->collider_atlas) {
        fprintf(stderr, "Failed to allocate memory for map collider_atlas atlas\n");
        return false;
    }
    for (int i = 0; i < WB_GRAPHIC_MAP_ATLAS_WIDTH * WB_GRAPHIC_MAP_ATLAS_HEIGHT; i++) {
        map->collider_atlas[i] = !!data[i * 4 + 3]; // Alpha channel
    }
    return true;
}

bool wbMapGetCollision(WBMap* map, int x, int y, int level) {
    return map->collider_atlas[(int)((level * WB_GRAPHIC_MAP_VIEW_HEIGHT + y) * WB_GRAPHIC_MAP_ATLAS_WIDTH + x + 0.5f)];
}