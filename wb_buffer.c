#include "wizball.h"

int wbBufferAppend(void* buffer, uint8_t object_type, WBVec2f* pos) {
    WBBufferHead* buffer_head = buffer;
    WBEntityHead* entity_head;
    int idx = 0;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        assert(buffer_head->cnt - 1 < WB_ENEMY_CNT_MAX);
        WBEnemy* enemy = ((WBEnemyBuffer*)buffer_head)->entries;
        while (enemy->head.type != WB_ENEMY_NONE) {
            enemy++;
            idx++;
        }
        entity_head = &enemy->head;
        break;

        case WB_BUFFER_PARTICLE:
        assert(buffer_head->cnt - 1 < WB_PARTICLE_CNT_MAX);
        WBParticle* particle = ((WBParticleBuffer*)buffer_head)->entries;
        while (particle->head.type != WB_PARTICLE_NONE) {
            particle++;
            idx++;
        }
        entity_head = &particle->head;
        break;

        case WB_BUFFER_PROJECTILE:
        assert(buffer_head->cnt - 1 < WB_PROJECTILE_CNT_MAX);
        WBProjectile* projectile = ((WBProjectileBuffer*)buffer_head)->entries;
        while (projectile->head.type != WB_PROJECTILE_NONE) {
            projectile++;
            idx++;
        }
        entity_head = &projectile->head;
        break;
    }
    entity_head->pos.x = pos->x;
    entity_head->pos.y = pos->y;
    entity_head->type = object_type;
    entity_head->color_key = 0.0f;
    entity_head->animation_key = 0.0f;

    buffer_head->cnt++;

    return idx;
}

void wbBufferRemove(void* buffer, int idx) {
    WBBufferHead* buffer_head = buffer;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        ((WBEnemyBuffer*)buffer_head)->entries[idx].head.type = WB_ENEMY_NONE;
        break;

        case WB_BUFFER_PARTICLE:
        ((WBParticleBuffer*)buffer_head)->entries[idx].head.type = WB_PARTICLE_NONE;
        break;

        case WB_BUFFER_PROJECTILE:
        ((WBProjectileBuffer*)buffer_head)->entries[idx].head.type = WB_PROJECTILE_NONE;
        break;
    }
    buffer_head->cnt--;
}

void wbBufferClear(void* buffer) {
    WBBufferHead* buffer_head = buffer;
    switch (buffer_head->type) {
        case WB_BUFFER_ENEMY:
        WBEnemy* entity = ((WBEnemyBuffer*)buffer_head)->entries;
        for (int i = 0; i < WB_ENEMY_CNT_MAX; i++) {
            entity++->head.type = WB_ENEMY_NONE;
        }
        break;

        case WB_BUFFER_PARTICLE:
        WBParticle* particle = ((WBParticleBuffer*)buffer_head)->entries;
        for (int i = 0; i < WB_PARTICLE_CNT_MAX; i++) {
            particle++->head.type = WB_PARTICLE_NONE;
        }
        break;

        case WB_BUFFER_PROJECTILE:
        WBProjectile* projectile = ((WBProjectileBuffer*)buffer_head)->entries;
        for (int i = 0; i < WB_PROJECTILE_CNT_MAX; i++) {
            projectile++->head.type = WB_PROJECTILE_NONE;
        }
        break;
    }
    buffer_head->cnt = 0;
}