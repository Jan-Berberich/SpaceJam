#include "wizball.h"

void* wbBufferAppend(void* head, uint8_t type, WBVec2f* pos) {
    WBBufferHead* buffer_head = head;
    WBEntityHead* entity_head;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        assert(buffer_head->cnt - 1 < WB_ENEMY_CNT_MAX);
        WBEnemy* enemy = ((WBBufferEnemy*)buffer_head)->entries;
        while (enemy->head.type != WB_ENEMY_NONE) {
            enemy++;
        }
        enemy->frame_age = 0;
        entity_head = &enemy->head;
        break;

        case WB_BUFFER_PARTICLE:
        assert(buffer_head->cnt - 1 < WB_PARTICLE_CNT_MAX);
        WBParticle* particle = ((WBBufferParticle*)buffer_head)->entries;
        while (particle->head.type != WB_PARTICLE_NONE) {
            particle++;
        }
        particle->frame_age = 0;
        entity_head = &particle->head;
        break;

        case WB_BUFFER_PROJECTILE:
        assert(buffer_head->cnt - 1 < WB_PROJECTILE_CNT_MAX);
        WBProjectile* projectile = ((WBProjectileBuffer*)buffer_head)->entries;
        while (projectile->head.type != WB_PROJECTILE_NONE) {
            projectile++;
        }
        entity_head = &projectile->head;
        break;
    }
    entity_head->pos.x = pos->x;
    entity_head->pos.y = pos->y;
    entity_head->type = type;
    entity_head->color_key = 0.0;

    buffer_head->cnt++;

    return entity_head;
}

void wbBufferRemove(void* head, int idx) {
    WBBufferHead* buffer_head = head;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        ((WBBufferEnemy*)buffer_head)->entries[idx].head.type = WB_ENEMY_NONE;
        break;

        case WB_BUFFER_PARTICLE:
        ((WBBufferParticle*)buffer_head)->entries[idx].head.type = WB_PARTICLE_NONE;
        break;

        case WB_BUFFER_PROJECTILE:
        ((WBProjectileBuffer*)buffer_head)->entries[idx].head.type = WB_PROJECTILE_NONE;
        break;
    }
    buffer_head->cnt--;
}