#include "wb.h"

void wbTextureInit(WBTexture* texture, uint8_t* data, int width, int height) {
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