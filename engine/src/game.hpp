#pragma once

#include <cstring>
#include "raymath_ext.h"
#include "platform.hpp"
#include "texture_2d.hpp"
#include "renderer_2d.hpp"
#include "entity.hpp"
#include <atomic>

struct game_state
{
    std::vector<Entity> entities;
    std::vector<Entity*> entities_sorted;
    std::atomic<unsigned int> nextID;
    unsigned int selected_entity = 0;
    bool g_Running;
};
