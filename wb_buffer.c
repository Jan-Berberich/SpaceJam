#include "wizball.h"

void wbBufferAppend(void* head, uint8_t type, float pos_x, float pos_y) {
    WBBufferHead* buffer_head = head;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        assert(buffer_head->cnt - 1 < WB_ENEMY_CNT_MAX);
        WBEnemy* entity = ((WBBufferEnemy*)buffer_head)->entries;
        while (entity->type != WB_ENEMY_NONE) {
            entity++;
        }
        entity->pos_x = pos_x;
        entity->pos_y = pos_y;
        entity->type = type;
        entity->animation_frame = -0.5f;
        break;

        case WB_BUFFER_PARTICLE:
        assert(buffer_head->cnt - 1 < WB_PARTICLE_CNT_MAX);
        WBParticle* particle = ((WBBufferParticle*)buffer_head)->entries;
        while (particle->type != WB_PARTICLE_NONE) {
            particle++;
        }
        particle->pos_x = pos_x;
        particle->pos_y = pos_y;
        particle->type = type;
        particle->animation_frame = -0.5f;
        break;
    }
    buffer_head->cnt++;
}

void wbBufferRemove(void* head, int idx) {
    WBBufferHead* buffer_head = head;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        ((WBBufferEnemy*)buffer_head)->entries[idx].type = WB_ENEMY_NONE;
        break;

        case WB_BUFFER_PARTICLE:
        ((WBBufferParticle*)buffer_head)->entries[idx].type = WB_PARTICLE_NONE;
        break;

        case WB_BUFFER_PROJECTILE:
        ((WBBufferProjectile*)buffer_head)->entries[idx].type = WB_PROJECTILE_NONE;
        break;
    }
    buffer_head->cnt--;
}