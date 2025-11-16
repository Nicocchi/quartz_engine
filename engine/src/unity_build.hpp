#pragma once

// Platform
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// System
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <map>
#include <queue>
#include <filesystem>

#include "raymath_ext.h"

// Header
#include "window.hpp"
#include "texture_2d.hpp"
#include "renderer_2d.hpp"
#include "editor.hpp"

// Class
#include "window.cpp"
#include "texture_2d.cpp"
#include "renderer_2d.cpp"
#include "editor.cpp"
