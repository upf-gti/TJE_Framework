#pragma once

#include "entity_enemy.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "game/entities/entity_player.h"


#include "framework/camera.h"
#include "framework/input.h"

#include "game/game.h"
#include "game/world.h"
#include <memory>

class EntityBullet : public EntityCollider {


	EntityBullet(Mesh* mesh, Material& mat, const std::string& name);

	std::vector<EntityBullet> bullets;

	//overwriten methods
	void render(Camera* camera);
	void update(float seconds_elapsed);

	void ithits();
};