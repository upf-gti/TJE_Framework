#pragma once

#include "framework/entities/entity_mesh.h"
#include "framework/animation.h"
#include "graphics/material.h"
#include "game/world.h"
#include "framework/input.h"

class EntityPlayer : public EntityMesh {

public:
	EntityPlayer();
	EntityPlayer(Mesh* mesh, const Material& material, const std::string& name = "");
	~EntityPlayer() {};

	// Vel in move the player
	Vector3 velocity;
	float healthbar = 100.0;
	float walk_speed = 0.5f;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float seconds_elapsed);
};
