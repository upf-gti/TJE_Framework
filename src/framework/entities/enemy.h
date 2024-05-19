#pragma once
#include "framework/entities/entityCollider.h"
#define HITBOX_RAD 0.2
#define PLAYER_HEIGHT 0.6

class Enemy : public EntityCollider {

public:
	Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP)
		: EntityCollider(mesh, material, ENEMY, name, is_dynamic) 
	{ 
		this->maxHP = HP; this->currHP = HP;
		this->type = COL_TYPE::ENEMY;
	}

	void render(Camera* camera);
	void update(float time_elapsed);

	bool touching_ground = false;
	bool colliding = false;
	float ground_below_y = 10000;
	float ground_y = 10000;
	Vector3 ground_normal;

	int maxHP;
	int currHP;

	Mesh* hitbox_mesh = Mesh::Get("data/meshes/sphere.obj");
	Shader* flat_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");

	Vector3 getPosition() const { return model.getTranslation(); };
	Vector3 getPositionGround() const { Vector3 res = getPosition(); res.y = 0; return res; }
	Vector3 getFront() { return model.frontVector(); }
	Vector3 getRight() { return model.rightVector(); }

	void showHitbox(Camera* camera);
	void sphere_bullet_collision(Vector3 position, float radius);
};

