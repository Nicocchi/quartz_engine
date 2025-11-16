bool LoadTexture(Texture2D *texture, const char *path, bool flip)
{
    int w, h, nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
    if (data == nullptr)
    {
        // error
        printf("Error loading texture: %s\n", path);
    }

    
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    if (nrChannels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    texture->width = w;
    texture->height = h;
    texture->isLoadedGPU = true;
    texture->path = path;
    return texture;
}

void BindTexture(Texture2D *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}

void DestroyTexture(Texture2D *texture)
{
    if (!texture) return;

    if (texture->ID)
    {
        glDeleteTextures(1, &texture->ID);
        texture->ID = 0;
    }
}