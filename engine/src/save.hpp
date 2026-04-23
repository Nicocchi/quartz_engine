#pragma once

#include <cstdio>
struct game_state;
struct Scene;
struct Sprite;
struct Tile;
struct Tilemap;
struct Entity;

void save_sprite(FILE *fp, const Sprite& s);
void load_sprite(FILE *fp, Sprite& s);
void save_tile(std::FILE *fp, const Tile& t);
void load_tile(std::FILE *fp, Tile& t);
void save_tilemap(std::FILE *fp, const Tilemap& tm);
void load_tilemap(std::FILE *fp, Tilemap& tm);
bool export_scene(game_state *state, const char *filename);
bool load_scene(game_state *state, const char *filename);