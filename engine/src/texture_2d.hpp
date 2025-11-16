#pragma once

struct Texture2D
{
    unsigned int ID;
    unsigned int width, height;
    bool isLoadedCPU;
    bool isLoadedGPU;
    const char *path;
};

bool LoadTexture(Texture2D *texture, const char *path, bool flip = false);
void BindTexture(Texture2D *texture);
void DestroyTexture(Texture2D *texture);