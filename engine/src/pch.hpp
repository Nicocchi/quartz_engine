#pragma once

#include <stdint.h>

#include "../vendor/glad/glad.h"
#include "../vendor/glad/glad.c"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#define IMGUI_IMPLEMENTATION
#define IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#define IMGUI_DEFINE_MATH_OPERATORS
// #define IMGUI_USE_STB_SPRINTF
#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/imgui/imgui_internal.h"

#include "../vendor/imgui/imgui_draw.cpp"
#include "../vendor/imgui/imgui_tables.cpp"
#include "../vendor/imgui/imgui_widgets.cpp"
#include "../vendor/imgui/imgui_impl_glfw.cpp"
#include "../vendor/imgui/imgui_impl_opengl3.cpp"
#include "../vendor/imgui/imgui.cpp"

#define GL_SILENCE_DEPRECATION
#include "../vendor/glfw/glfw3.h"
