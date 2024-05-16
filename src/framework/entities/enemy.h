#pragma once
#include "framework/entities/entityCollider.h"

class Enemy : public EntityCollider {

public:
	Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic)
		: EntityCollider(mesh, material, name, is_dynamic) {}

	void render(Camera* camera);
	void update(float time_elapsed) {};
};

