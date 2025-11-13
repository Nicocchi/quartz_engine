bool LoadTexture(Texture2D *texture, const char *path)
{
    int w, h, nrChannels;
    unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
    if (data == nullptr)
    {
        // error
        printf("Error loading texture: %s\n", path);
    }

    texture->width = w;
    texture->height = h;
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

    // glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
    // // char *subimg = (char*)data + (10 + 10 * w) * 4;
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 10, 10, 32, 32, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    stbi_image_free(data);
    texture->isLoadedGPU = true;
    return texture;
}

void BindTexture(Texture2D *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}
