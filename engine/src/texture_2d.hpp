#pragma once

#include <string>

struct Texture2D
{
    unsigned int ID;
    unsigned int width, height;
    bool isLoadedCPU;
    bool isLoadedGPU;
    // const char *path;
    std::string path;
};

bool LoadTexture(Texture2D *texture, const char *path, bool flip = false);
void BindTexture(Texture2D *texture);
void DestroyTexture(Texture2D *texture);