#pragma once

#include "framework/includes.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"

class World {
public:
    static World* instance;
    std::vector<Entity*> entities;

    // Method to add or delete entities
    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);

    // Method to redner all entities, this should be done calling just one function on entity since we will do the tree approach
    void renderAll(Camera* camera);

    // Method to update all entities
    void updateAll(float delta_time);
};
