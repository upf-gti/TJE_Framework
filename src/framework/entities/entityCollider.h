#pragma once

#include "framework/entities/entityMesh.h"

#include "graphics/material.h"


class EntityCollider : public EntityMesh {

public:
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

	struct sCollisionData {
		Vector3 colPoint;
		Vector3 colNormal;
	};


	bool is_dynamic = false;
};


