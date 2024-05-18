#pragma once
#include "framework/entities/entityCollider.h"

class Enemy : public EntityCollider {

public:
	Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP)
		: EntityCollider(mesh, material, ENEMY, name, is_dynamic) { this->maxHP = HP; this->currHP = HP; }

	void render(Camera* camera);
	void update(float time_elapsed) {};

	int maxHP;
	int currHP;
};

