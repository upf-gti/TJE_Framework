#include "world.h"

World* World::instance = NULL;

void World::addEntity(Entity* entity) {
	entities.push_back(entity);
}

void World::removeEntity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}