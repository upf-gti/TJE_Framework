#pragma once

#include "framework/entities/entityMesh.h"

#define HITBOX_RAD 0.2
#define PLAYER_HEIGHT 0.6

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};

enum {
	NONE = 0,
	FLOOR = 1 << 0,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	ENEMY = 1 << 3,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR,
	ALL = 0xFF
};

class EntityCollider : public EntityMesh {

public:
	EntityCollider() {}
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "", bool is_dynamic=false);
	bool is_dynamic = false;
};