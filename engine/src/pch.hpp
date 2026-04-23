#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <atomic>
#include <cstring>
#include <stdint.h>
#include <filesystem>

// Heavy vendor headers that never change
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/imgui_stdlib.h"
#include "../vendor/glad/glad.h"
#include "../vendor/glfw/glfw3.h"
#include "raymath_ext.h"
#include "../vendor/uuid_v4.h"