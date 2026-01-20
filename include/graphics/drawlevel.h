#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H

#include "level/maploader.h"
#include "core/entities.h"

void drawLevel(const MapLoader& map);
void drawEntities(const std::vector<Enemy>& enemies, const std::vector<Item>& items, float camX, float camZ);
#endif
