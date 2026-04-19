void save_sprite(FILE *fp, const Sprite& s)
{
    uint32_t len = (uint32_t)s.filename.size();
    std::fwrite(&len, sizeof(uint32_t), 1, fp);
    std::fwrite(s.filename.data(), 1, len, fp);

    std::string log = "Save Sprite: " + s.filename;
    printf(log.c_str());
    game_logs.messages.push_back(log);

    std::fwrite(&s.color, sizeof(Vector4), 1, fp);
    std::fwrite(&s.z_index, sizeof(int), 1, fp);
    std::fwrite(&s.flip_x, sizeof(bool), 1, fp);
    std::fwrite(&s.flip_y, sizeof(bool), 1, fp);
}

void load_sprite(FILE *fp, Sprite& s)
{
    uint32_t len;
    std::fread(&len, sizeof(uint32_t), 1, fp);

    s.filename.resize(len);
    std::fread(s.filename.data(), 1, len, fp);

    std::string log = "Load Sprite: " + s.filename;
    printf(log.c_str());
    game_logs.messages.push_back(log);

    std::fread(&s.color, sizeof(Vector4), 1, fp);
    std::fread(&s.z_index, sizeof(int), 1, fp);
    std::fread(&s.flip_x, sizeof(bool), 1, fp);
    std::fread(&s.flip_y, sizeof(bool), 1, fp);

    s.texture = new Texture2D();

    if (!LoadTexture(s.texture, s.filename.c_str()))
    {
        std::string log = "Loading Sprite: Failed to load texture: %s\n" + s.filename;
        printf(log.c_str());
        game_logs.messages.push_back(log);
        delete s.texture;
        s.texture = nullptr;
    }
}

void save_tile(std::FILE *fp, const Tile& t)
{
    std::fwrite(&t.id, sizeof(unsigned int), 1, fp);

    // save_sprite(fp, t.sprite);

    std::fwrite(&t.transform, sizeof(Transform), 1, fp);

    std::fwrite(&t.u0, sizeof(float), 1, fp);
    std::fwrite(&t.u1, sizeof(float), 1, fp);
    std::fwrite(&t.v0, sizeof(float), 1, fp);
    std::fwrite(&t.v1, sizeof(float), 1, fp);

    std::fwrite(&t.index, sizeof(int), 1, fp);
}

void load_tile(std::FILE *fp, Tile& t)
{
    std::fread(&t.id, sizeof(unsigned int), 1, fp);

    // load_sprite(fp, t.sprite);

    std::fread(&t.transform, sizeof(Transform), 1, fp);

    std::fread(&t.u0, sizeof(float), 1, fp);
    std::fread(&t.u1, sizeof(float), 1, fp);
    std::fread(&t.v0, sizeof(float), 1, fp);
    std::fread(&t.v1, sizeof(float), 1, fp);

    std::fread(&t.index, sizeof(int), 1, fp);
}

void save_tilemap(std::FILE *fp, const Tilemap& tm)
{
    std::fwrite(&tm.id, sizeof(unsigned int), 1, fp);
    std::fwrite(&tm.cell_width, sizeof(int), 1, fp);
    std::fwrite(&tm.cell_height, sizeof(int), 1, fp);
    std::fwrite(&tm.texture_width, sizeof(int), 1, fp);
    std::fwrite(&tm.texture_height, sizeof(int), 1, fp);
    std::fwrite(&tm.enabled, sizeof(bool), 1, fp);

    // save_sprite(fp, tm.sprite);

    // Save tiles vector
    size_t tileCount = tm.tiles.size();
    std::fwrite(&tileCount, sizeof(size_t), 1, fp);


    for (const Tile& t : tm.tiles)
    {
        // std::fwrite(&t, sizeof(Tile), 1, fp);
        save_tile(fp, t);
    }

    // Save map
    size_t mapCount = tm.map.size();
    std::fwrite(&mapCount, sizeof(size_t), 1, fp);

    for (const auto& pair : tm.map)
    {
        std::fwrite(&pair.first, sizeof(GridCoord), 1, fp);
        save_tile(fp, pair.second);
        // std::fwrite(&pair.second, sizeof(Tile), 1, fp);
    }
}

void load_tilemap(std::FILE *fp, Tilemap& tm)
{
    std::fread(&tm.id, sizeof(unsigned int), 1, fp);
    std::fread(&tm.cell_width, sizeof(int), 1, fp);
    std::fread(&tm.cell_height, sizeof(int), 1, fp);
    std::fread(&tm.texture_width, sizeof(int), 1, fp);
    std::fread(&tm.texture_height, sizeof(int), 1, fp);
    std::fread(&tm.enabled, sizeof(bool), 1, fp);

    // load_sprite(fp, tm.sprite);

    // Load tiles
    size_t tileCount;
    std::fread(&tileCount, sizeof(size_t), 1, fp);

    tm.tiles.resize(tileCount);

    for (size_t i = 0; i < tileCount; i++)
    {
        // std::fread(&tm.tiles[i], sizeof(Tile), 1, fp);
        load_tile(fp, tm.tiles[i]);
    }

    // Load map
    size_t mapCount;
    std::fread(&mapCount, sizeof(size_t), 1, fp);

    tm.map.clear();

    for (size_t i = 0; i < mapCount; i++)
    {
        GridCoord key;
        Tile value;

        std::fread(&key, sizeof(GridCoord), 1, fp);
        // std::fread(&value, sizeof(Tile), 1, fp);
        load_tile(fp, value);

        tm.map[key] = value;
    }
}

bool export_scene(game_state *state, const char *filename)
{
    std::string path = "./assets/";
    path += filename;
    path += ".bin";
    std::FILE *fp = std::fopen(path.c_str(), "wb");
    if (!fp) {
        return false;
    }

    std::fwrite(&state->nextID, sizeof(unsigned int), 1, fp);
    std::fwrite(&state->scenes[state->current_scene].selected_entity, sizeof(unsigned int), 1, fp);
    std::fwrite(&state->scenes[state->current_scene].selected_tile, sizeof(unsigned int), 1, fp);

    size_t count = state->scenes[state->current_scene].entities.size();
    std::fwrite(&count, sizeof(size_t), 1, fp);

    for (const Entity& e : state->scenes[state->current_scene].entities)
    {
        std::fwrite(&e.id, sizeof(unsigned int), 1, fp);

        size_t len = e.name.size();
        std::fwrite(&len, sizeof(size_t), 1, fp);
        std::fwrite(e.name.data(), sizeof(char), len, fp);

        std::fwrite(&e.vertexOffset, sizeof(size_t), 1, fp);
        std::fwrite(&e.transform, sizeof(Transform), 1, fp);
        std::fwrite(&e.local, sizeof(Transform), 1, fp);
        std::fwrite(&e.world, sizeof(Transform), 1, fp);
        std::fwrite(&e.dirty, sizeof(bool), 1, fp);

        // std::fwrite(&e.sprite, sizeof(Sprite), 1, fp);
        save_sprite(fp, e.sprite);
        std::fwrite(&e.animated_sprite, sizeof(AnimatedSprite), 1, fp);
        // std::fwrite(&e.tilemap, sizeof(Tilemap), 1, fp);
        save_tilemap(fp, e.tilemap);

        std::fwrite(&e.flags, sizeof(flag_t), 1, fp);
    }

    std::fclose(fp);

    return true;
}

bool load_scene(game_state *state, const char *filename)
{
    std::string path = "./assets/";
    path += filename;
    path += ".bin";
    std::FILE *fp = fopen(path.c_str(), "rb");
    std::string log = "Loading Scene: " + path;
    printf(log.c_str());
    game_logs.messages.push_back(log);
    if (!fp)
    {
        log = "Failed to load scene: " + path;
        printf(log.c_str());
        game_logs.messages.push_back(log);
        return false;
    }

    Scene new_scene;


    std::fread(&state->nextID, sizeof(unsigned int), 1, fp);
    std::fread(&state->scenes[state->current_scene].selected_entity, sizeof(unsigned int), 1, fp);
    std::fread(&state->scenes[state->current_scene].selected_tile, sizeof(unsigned int), 1, fp);
    state->scenes[state->current_scene].selected_entity = 0;
    state->scenes[state->current_scene].selected_tile = 0;

    size_t count;
    std::fread(&count, sizeof(size_t), 1, fp);

    state->scenes[state->current_scene].entities.clear();
    state->scenes[state->current_scene].entities.resize(count);

    for (size_t i = 0; i < count; i++)
    {
        Entity& e = state->scenes[state->current_scene].entities[i];

        std::fread(&e.id, sizeof(unsigned int), 1, fp);

        size_t len;
        std::fread(&len, sizeof(size_t), 1, fp);

        e.name.resize(len);
        std::fread(e.name.data(), sizeof(char), len, fp);

        std::string log = "Loading entity: " + e.name;
        printf(log.c_str());
        game_logs.messages.push_back(log);

        std::fread(&e.vertexOffset, sizeof(size_t), 1, fp);
        std::fread(&e.transform, sizeof(Transform), 1, fp);
        std::fread(&e.local, sizeof(Transform), 1, fp);
        std::fread(&e.world, sizeof(Transform), 1, fp);
        std::fread(&e.dirty, sizeof(bool), 1, fp);

        load_sprite(fp, e.sprite);
        std::fread(&e.animated_sprite, sizeof(AnimatedSprite), 1, fp);
        load_tilemap(fp, e.tilemap);

        std::fread(&e.flags, sizeof(flag_t), 1, fp);
    }

    std::fclose(fp);
    return true;
}