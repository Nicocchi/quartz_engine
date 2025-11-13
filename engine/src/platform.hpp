#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct arena
{
    void *base;
    uint32_t size;
    uint32_t used;
} arena;

typedef struct game_memory
{
    void* storage;
    uint64_t storageSize;
    void *renderStorage;
    uint64_t renderSize;
    bool isInit;
} game_memory;

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *GameMemory, float deltaTime)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);


#ifdef __cplusplus
}
#endif