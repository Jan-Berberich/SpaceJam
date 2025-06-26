#include "wizball.h"

void wbGraphicTextureInit(WBTexture* texture, uint8_t* data, int width, int height) {
    texture->width = width;
    texture->height = height;

    glGenTextures(1, &texture->texture_id); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, texture->texture_id); // Bind the texture
    // Upload image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for scaling

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge to avoid border artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // filtering for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // filtering for magnification
}

bool wbGraphicInit(WBGraphic* graphic, WBMap* map) {
    map->graphic_handle = graphic;
    int width, height, channel_cnt;
    uint8_t* data;

    // sprite atlas
    data = stbi_load(WB_SPRITE_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load sprite atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->sprite_atlas, data, width, height);
    stbi_image_free(data);
    
    // background atlas
    data = stbi_load(WB_MAP_BACKGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map background atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->background_atlas, data, width, height);
    stbi_image_free(data);

    // foreground atlas
    data = stbi_load(WB_MAP_FOREGROUND_ATLAS_PATH, &width, &height, &channel_cnt, 4);
    if (!data) {
        fprintf(stderr, "Failed to load map foreground atlas image\n");
        wbGraphicUninit(graphic, map);
        return false;
    }
    wbGraphicTextureInit(&graphic->foreground_atlas, data, width, height);
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
    graphic->dust_texture.width = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE * WB_GRAPHIC_MAP_DUST_LAYER_CNT;
    graphic->dust_texture.height = WB_GRAPHIC_MAP_DUST_SPRITE_SIZE;
    wbGraphicTextureInit(&graphic->dust_texture, data, graphic->dust_texture.width, graphic->dust_texture.height);
    free(data);

    // colormap
    graphic->colorpallet.enemy[0] = WB_GRAPHIC_COLORPALLET_ENEMY_0;
    graphic->colorpallet.enemy[1] = WB_GRAPHIC_COLORPALLET_ENEMY_1;
    graphic->colorpallet.enemy[2] = WB_GRAPHIC_COLORPALLET_ENEMY_2;
    graphic->colorpallet.enemy[3] = WB_GRAPHIC_COLORPALLET_ENEMY_3;
    graphic->colorpallet.enemy[4] = WB_GRAPHIC_COLORPALLET_ENEMY_4;
    graphic->colorpallet.enemy[5] = WB_GRAPHIC_COLORPALLET_ENEMY_5;
    graphic->colorpallet.beam[0] = WB_GRAPHIC_COLORPALLET_BEAM_0;
    graphic->colorpallet.beam[1] = WB_GRAPHIC_COLORPALLET_BEAM_1;
    graphic->colorpallet.beam[2] = WB_GRAPHIC_COLORPALLET_BEAM_2;
    graphic->colorpallet.beam[3] = WB_GRAPHIC_COLORPALLET_BEAM_3;
    graphic->colorpallet.beam[4] = WB_GRAPHIC_COLORPALLET_BEAM_4;
    graphic->colorpallet.red8[0] = WB_GRAPHIC_COLORPALLET_RED8_0;
    graphic->colorpallet.red8[1] = WB_GRAPHIC_COLORPALLET_RED8_1;
    graphic->colorpallet.red8[2] = WB_GRAPHIC_COLORPALLET_RED8_2;
    graphic->colorpallet.red8[3] = WB_GRAPHIC_COLORPALLET_RED8_3;
    graphic->colorpallet.red8[4] = WB_GRAPHIC_COLORPALLET_RED8_4;
    graphic->colorpallet.red8[5] = WB_GRAPHIC_COLORPALLET_RED8_5;
    graphic->colorpallet.red8[6] = WB_GRAPHIC_COLORPALLET_RED8_6;
    graphic->colorpallet.red8[7] = WB_GRAPHIC_COLORPALLET_RED8_7;
    graphic->colorpallet.green8[0] = WB_GRAPHIC_COLORPALLET_GREEN8_0;
    graphic->colorpallet.green8[1] = WB_GRAPHIC_COLORPALLET_GREEN8_1;
    graphic->colorpallet.green8[2] = WB_GRAPHIC_COLORPALLET_GREEN8_2;
    graphic->colorpallet.green8[3] = WB_GRAPHIC_COLORPALLET_GREEN8_3;
    graphic->colorpallet.green8[4] = WB_GRAPHIC_COLORPALLET_GREEN8_4;
    graphic->colorpallet.green8[5] = WB_GRAPHIC_COLORPALLET_GREEN8_5;
    graphic->colorpallet.green8[6] = WB_GRAPHIC_COLORPALLET_GREEN8_6;
    graphic->colorpallet.green8[7] = WB_GRAPHIC_COLORPALLET_GREEN8_7;
    graphic->colorpallet.blue8[0] = WB_GRAPHIC_COLORPALLET_BLUE8_0;
    graphic->colorpallet.blue8[1] = WB_GRAPHIC_COLORPALLET_BLUE8_1;
    graphic->colorpallet.blue8[2] = WB_GRAPHIC_COLORPALLET_BLUE8_2;
    graphic->colorpallet.blue8[3] = WB_GRAPHIC_COLORPALLET_BLUE8_3;
    graphic->colorpallet.blue8[4] = WB_GRAPHIC_COLORPALLET_BLUE8_4;
    graphic->colorpallet.blue8[5] = WB_GRAPHIC_COLORPALLET_BLUE8_5;
    graphic->colorpallet.blue8[6] = WB_GRAPHIC_COLORPALLET_BLUE8_6;
    graphic->colorpallet.blue8[7] = WB_GRAPHIC_COLORPALLET_BLUE8_7;
    graphic->colorpallet.blue6[0] = WB_GRAPHIC_COLORPALLET_BLUE6_0;
    graphic->colorpallet.blue6[1] = WB_GRAPHIC_COLORPALLET_BLUE6_1;
    graphic->colorpallet.blue6[2] = WB_GRAPHIC_COLORPALLET_BLUE6_2;
    graphic->colorpallet.blue6[3] = WB_GRAPHIC_COLORPALLET_BLUE6_3;
    graphic->colorpallet.blue6[4] = WB_GRAPHIC_COLORPALLET_BLUE6_4;
    graphic->colorpallet.blue6[5] = WB_GRAPHIC_COLORPALLET_BLUE6_5;
    graphic->colorpallet.all32[ 0] = WB_GRAPHIC_COLORPALLET_ALL32_0;
    graphic->colorpallet.all32[ 1] = WB_GRAPHIC_COLORPALLET_ALL32_1;
    graphic->colorpallet.all32[ 2] = WB_GRAPHIC_COLORPALLET_ALL32_2;
    graphic->colorpallet.all32[ 3] = WB_GRAPHIC_COLORPALLET_ALL32_3;
    graphic->colorpallet.all32[ 4] = WB_GRAPHIC_COLORPALLET_ALL32_4;
    graphic->colorpallet.all32[ 5] = WB_GRAPHIC_COLORPALLET_ALL32_5;
    graphic->colorpallet.all32[ 6] = WB_GRAPHIC_COLORPALLET_ALL32_6;
    graphic->colorpallet.all32[ 7] = WB_GRAPHIC_COLORPALLET_ALL32_7;
    graphic->colorpallet.all32[ 8] = WB_GRAPHIC_COLORPALLET_ALL32_8;
    graphic->colorpallet.all32[ 9] = WB_GRAPHIC_COLORPALLET_ALL32_9;
    graphic->colorpallet.all32[10] = WB_GRAPHIC_COLORPALLET_ALL32_10;
    graphic->colorpallet.all32[11] = WB_GRAPHIC_COLORPALLET_ALL32_11;
    graphic->colorpallet.all32[12] = WB_GRAPHIC_COLORPALLET_ALL32_12;
    graphic->colorpallet.all32[13] = WB_GRAPHIC_COLORPALLET_ALL32_13;
    graphic->colorpallet.all32[14] = WB_GRAPHIC_COLORPALLET_ALL32_14;
    graphic->colorpallet.all32[15] = WB_GRAPHIC_COLORPALLET_ALL32_15;
    graphic->colorpallet.all32[16] = WB_GRAPHIC_COLORPALLET_ALL32_16;
    graphic->colorpallet.all32[17] = WB_GRAPHIC_COLORPALLET_ALL32_17;
    graphic->colorpallet.all32[18] = WB_GRAPHIC_COLORPALLET_ALL32_18;
    graphic->colorpallet.all32[19] = WB_GRAPHIC_COLORPALLET_ALL32_19;
    graphic->colorpallet.all32[20] = WB_GRAPHIC_COLORPALLET_ALL32_20;
    graphic->colorpallet.all32[21] = WB_GRAPHIC_COLORPALLET_ALL32_21;
    graphic->colorpallet.all32[22] = WB_GRAPHIC_COLORPALLET_ALL32_22;
    graphic->colorpallet.all32[23] = WB_GRAPHIC_COLORPALLET_ALL32_23;
    graphic->colorpallet.all32[24] = WB_GRAPHIC_COLORPALLET_ALL32_24;
    graphic->colorpallet.all32[25] = WB_GRAPHIC_COLORPALLET_ALL32_25;
    graphic->colorpallet.all32[26] = WB_GRAPHIC_COLORPALLET_ALL32_26;
    graphic->colorpallet.all32[27] = WB_GRAPHIC_COLORPALLET_ALL32_27;
    graphic->colorpallet.all32[28] = WB_GRAPHIC_COLORPALLET_ALL32_28;
    graphic->colorpallet.all32[29] = WB_GRAPHIC_COLORPALLET_ALL32_29;
    graphic->colorpallet.all32[30] = WB_GRAPHIC_COLORPALLET_ALL32_30;
    graphic->colorpallet.all32[31] = WB_GRAPHIC_COLORPALLET_ALL32_31;
    graphic->colorpallet.green4[0] = WB_GRAPHIC_COLORPALLET_GREEN4_0;
    graphic->colorpallet.green4[1] = WB_GRAPHIC_COLORPALLET_GREEN4_1;
    graphic->colorpallet.green4[2] = WB_GRAPHIC_COLORPALLET_GREEN4_2;
    graphic->colorpallet.green4[3] = WB_GRAPHIC_COLORPALLET_GREEN4_3;
    graphic->colorpallet.blue4[0] = WB_GRAPHIC_COLORPALLET_BLUE4_0;
    graphic->colorpallet.blue4[1] = WB_GRAPHIC_COLORPALLET_BLUE4_1;
    graphic->colorpallet.blue4[2] = WB_GRAPHIC_COLORPALLET_BLUE4_2;
    graphic->colorpallet.blue4[3] = WB_GRAPHIC_COLORPALLET_BLUE4_3;
    graphic->colorpallet.red4[0] = WB_GRAPHIC_COLORPALLET_RED4_0;
    graphic->colorpallet.red4[1] = WB_GRAPHIC_COLORPALLET_RED4_1;
    graphic->colorpallet.red4[2] = WB_GRAPHIC_COLORPALLET_RED4_2;
    graphic->colorpallet.red4[3] = WB_GRAPHIC_COLORPALLET_RED4_3;

    return true;
}

void wbGraphicUninit(WBGraphic* graphic, WBMap* map) {
    if (graphic->sprite_atlas.texture_id) glDeleteTextures(1, &graphic->sprite_atlas.texture_id);
    if (graphic->background_atlas.texture_id) glDeleteTextures(1, &graphic->background_atlas.texture_id);
    if (graphic->foreground_atlas.texture_id) glDeleteTextures(1, &graphic->foreground_atlas.texture_id);
    if (graphic->dust_texture.texture_id) glDeleteTextures(1, &graphic->dust_texture.texture_id);
    if (map->collider_atlas) free(map->collider_atlas);
}