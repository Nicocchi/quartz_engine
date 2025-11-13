#pragma once

struct Texture2D
{
    unsigned int ID;
    unsigned int width, height;
    bool isLoadedCPU;
    bool isLoadedGPU;
};

bool LoadTexture(Texture2D *texture, const char *path);
void BindTexture(Texture2D *texture);
