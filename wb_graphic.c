#include "wizball.h"

void wbGraphicTextureInit(GLuint* texture_id, uint8_t* data, int width, int height) {
    glGenTextures(1, texture_id); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, *texture_id); // Bind the texture
    // Upload image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for scaling

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge to avoid border artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // filtering for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // filtering for magnification
}

bool wbGraphicInit(WBGraphic* graphic, WBMap* map) {
    int width, height, channel_cnt;
    uint8_t* data;

    // sprite atlas
    data = stbi_load(WB_SPRITE_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load sprite atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->sprite_atlas_texture_id, data, width, height);
    stbi_image_free(data);
    
    // background atlas
    data = stbi_load(WB_MAP_BACKGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->background_atlas_texture_id, data, width, height);
    stbi_image_free(data);

    // foreground atlas
    data = stbi_load(WB_MAP_FOREGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map foreground atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->foreground_atlas_texture_id, data, width, height);
    if (!wbMapInitCollider(map, data)) {
        stbi_image_free(data);
        wbGraphicUninit(graphic, map);
        return false;
    }
    stbi_image_free(data);

    // dust texture
    data = malloc(WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT * 4 * (sizeof *data));
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for dust texture\n");
        wbGraphicUninit(graphic, map);
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
    wbGraphicTextureInit(&graphic->dust_atlas_texture_id, data, WB_GRAPHIC_MAP_DUST_ATLAS_WIDTH, WB_GRAPHIC_MAP_DUST_ATLAS_HEIGHT);
    free(data);

    // colormap
    graphic->colormap.enemy[0] = WB_GRAPHIC_COLORMAP_ENEMY_0;
    graphic->colormap.enemy[1] = WB_GRAPHIC_COLORMAP_ENEMY_1;
    graphic->colormap.enemy[2] = WB_GRAPHIC_COLORMAP_ENEMY_2;
    graphic->colormap.enemy[3] = WB_GRAPHIC_COLORMAP_ENEMY_3;
    graphic->colormap.enemy[4] = WB_GRAPHIC_COLORMAP_ENEMY_4;
    graphic->colormap.enemy[5] = WB_GRAPHIC_COLORMAP_ENEMY_5;
    graphic->colormap.beam[0] = WB_GRAPHIC_COLORMAP_BEAM_0;
    graphic->colormap.beam[1] = WB_GRAPHIC_COLORMAP_BEAM_1;
    graphic->colormap.beam[2] = WB_GRAPHIC_COLORMAP_BEAM_2;
    graphic->colormap.beam[3] = WB_GRAPHIC_COLORMAP_BEAM_3;
    graphic->colormap.beam[4] = WB_GRAPHIC_COLORMAP_BEAM_4;
    graphic->colormap.red8[0] = WB_GRAPHIC_COLORMAP_RED8_0;
    graphic->colormap.red8[1] = WB_GRAPHIC_COLORMAP_RED8_1;
    graphic->colormap.red8[2] = WB_GRAPHIC_COLORMAP_RED8_2;
    graphic->colormap.red8[3] = WB_GRAPHIC_COLORMAP_RED8_3;
    graphic->colormap.red8[4] = WB_GRAPHIC_COLORMAP_RED8_4;
    graphic->colormap.red8[5] = WB_GRAPHIC_COLORMAP_RED8_5;
    graphic->colormap.red8[6] = WB_GRAPHIC_COLORMAP_RED8_6;
    graphic->colormap.red8[7] = WB_GRAPHIC_COLORMAP_RED8_7;
    graphic->colormap.green8[0] = WB_GRAPHIC_COLORMAP_GREEN8_0;
    graphic->colormap.green8[1] = WB_GRAPHIC_COLORMAP_GREEN8_1;
    graphic->colormap.green8[2] = WB_GRAPHIC_COLORMAP_GREEN8_2;
    graphic->colormap.green8[3] = WB_GRAPHIC_COLORMAP_GREEN8_3;
    graphic->colormap.green8[4] = WB_GRAPHIC_COLORMAP_GREEN8_4;
    graphic->colormap.green8[5] = WB_GRAPHIC_COLORMAP_GREEN8_5;
    graphic->colormap.green8[6] = WB_GRAPHIC_COLORMAP_GREEN8_6;
    graphic->colormap.green8[7] = WB_GRAPHIC_COLORMAP_GREEN8_7;
    graphic->colormap.blue8[0] = WB_GRAPHIC_COLORMAP_BLUE8_0;
    graphic->colormap.blue8[1] = WB_GRAPHIC_COLORMAP_BLUE8_1;
    graphic->colormap.blue8[2] = WB_GRAPHIC_COLORMAP_BLUE8_2;
    graphic->colormap.blue8[3] = WB_GRAPHIC_COLORMAP_BLUE8_3;
    graphic->colormap.blue8[4] = WB_GRAPHIC_COLORMAP_BLUE8_4;
    graphic->colormap.blue8[5] = WB_GRAPHIC_COLORMAP_BLUE8_5;
    graphic->colormap.blue8[6] = WB_GRAPHIC_COLORMAP_BLUE8_6;
    graphic->colormap.blue8[7] = WB_GRAPHIC_COLORMAP_BLUE8_7;
    graphic->colormap.blue6[0] = WB_GRAPHIC_COLORMAP_BLUE6_0;
    graphic->colormap.blue6[1] = WB_GRAPHIC_COLORMAP_BLUE6_1;
    graphic->colormap.blue6[2] = WB_GRAPHIC_COLORMAP_BLUE6_2;
    graphic->colormap.blue6[3] = WB_GRAPHIC_COLORMAP_BLUE6_3;
    graphic->colormap.blue6[4] = WB_GRAPHIC_COLORMAP_BLUE6_4;
    graphic->colormap.blue6[5] = WB_GRAPHIC_COLORMAP_BLUE6_5;
    graphic->colormap.all32[ 0] = WB_GRAPHIC_COLORMAP_ALL32_0;
    graphic->colormap.all32[ 1] = WB_GRAPHIC_COLORMAP_ALL32_1;
    graphic->colormap.all32[ 2] = WB_GRAPHIC_COLORMAP_ALL32_2;
    graphic->colormap.all32[ 3] = WB_GRAPHIC_COLORMAP_ALL32_3;
    graphic->colormap.all32[ 4] = WB_GRAPHIC_COLORMAP_ALL32_4;
    graphic->colormap.all32[ 5] = WB_GRAPHIC_COLORMAP_ALL32_5;
    graphic->colormap.all32[ 6] = WB_GRAPHIC_COLORMAP_ALL32_6;
    graphic->colormap.all32[ 7] = WB_GRAPHIC_COLORMAP_ALL32_7;
    graphic->colormap.all32[ 8] = WB_GRAPHIC_COLORMAP_ALL32_8;
    graphic->colormap.all32[ 9] = WB_GRAPHIC_COLORMAP_ALL32_9;
    graphic->colormap.all32[10] = WB_GRAPHIC_COLORMAP_ALL32_10;
    graphic->colormap.all32[11] = WB_GRAPHIC_COLORMAP_ALL32_11;
    graphic->colormap.all32[12] = WB_GRAPHIC_COLORMAP_ALL32_12;
    graphic->colormap.all32[13] = WB_GRAPHIC_COLORMAP_ALL32_13;
    graphic->colormap.all32[14] = WB_GRAPHIC_COLORMAP_ALL32_14;
    graphic->colormap.all32[15] = WB_GRAPHIC_COLORMAP_ALL32_15;
    graphic->colormap.all32[16] = WB_GRAPHIC_COLORMAP_ALL32_16;
    graphic->colormap.all32[17] = WB_GRAPHIC_COLORMAP_ALL32_17;
    graphic->colormap.all32[18] = WB_GRAPHIC_COLORMAP_ALL32_18;
    graphic->colormap.all32[19] = WB_GRAPHIC_COLORMAP_ALL32_19;
    graphic->colormap.all32[20] = WB_GRAPHIC_COLORMAP_ALL32_20;
    graphic->colormap.all32[21] = WB_GRAPHIC_COLORMAP_ALL32_21;
    graphic->colormap.all32[22] = WB_GRAPHIC_COLORMAP_ALL32_22;
    graphic->colormap.all32[23] = WB_GRAPHIC_COLORMAP_ALL32_23;
    graphic->colormap.all32[24] = WB_GRAPHIC_COLORMAP_ALL32_24;
    graphic->colormap.all32[25] = WB_GRAPHIC_COLORMAP_ALL32_25;
    graphic->colormap.all32[26] = WB_GRAPHIC_COLORMAP_ALL32_26;
    graphic->colormap.all32[27] = WB_GRAPHIC_COLORMAP_ALL32_27;
    graphic->colormap.all32[28] = WB_GRAPHIC_COLORMAP_ALL32_28;
    graphic->colormap.all32[29] = WB_GRAPHIC_COLORMAP_ALL32_29;
    graphic->colormap.all32[30] = WB_GRAPHIC_COLORMAP_ALL32_30;
    graphic->colormap.all32[31] = WB_GRAPHIC_COLORMAP_ALL32_31;
    graphic->colormap.green4[0] = WB_GRAPHIC_COLORMAP_GREEN4_0;
    graphic->colormap.green4[1] = WB_GRAPHIC_COLORMAP_GREEN4_1;
    graphic->colormap.green4[2] = WB_GRAPHIC_COLORMAP_GREEN4_2;
    graphic->colormap.green4[3] = WB_GRAPHIC_COLORMAP_GREEN4_3;
    graphic->colormap.blue4[0] = WB_GRAPHIC_COLORMAP_BLUE4_0;
    graphic->colormap.blue4[1] = WB_GRAPHIC_COLORMAP_BLUE4_1;
    graphic->colormap.blue4[2] = WB_GRAPHIC_COLORMAP_BLUE4_2;
    graphic->colormap.blue4[3] = WB_GRAPHIC_COLORMAP_BLUE4_3;
    graphic->colormap.red4[0] = WB_GRAPHIC_COLORMAP_RED4_0;
    graphic->colormap.red4[1] = WB_GRAPHIC_COLORMAP_RED4_1;
    graphic->colormap.red4[2] = WB_GRAPHIC_COLORMAP_RED4_2;
    graphic->colormap.red4[3] = WB_GRAPHIC_COLORMAP_RED4_3;

    return true;
}

void wbGraphicUninit(WBGraphic* graphic, WBMap* map) {
    if (graphic->sprite_atlas_texture_id) glDeleteTextures(1, &graphic->sprite_atlas_texture_id);
    if (graphic->background_atlas_texture_id) glDeleteTextures(1, &graphic->background_atlas_texture_id);
    if (graphic->foreground_atlas_texture_id) glDeleteTextures(1, &graphic->foreground_atlas_texture_id);
    if (graphic->dust_atlas_texture_id) glDeleteTextures(1, &graphic->dust_atlas_texture_id);
    if (map->collider_atlas) free(map->collider_atlas);
}