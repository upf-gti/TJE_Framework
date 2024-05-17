#pragma once

#include "framework/entities/entityMesh.h"

#include "graphics/material.h"


class EntityCollider : public EntityMesh {

public:
	enum col_type {
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

	col_type type;

	EntityCollider() {
		type = NONE;
	};
	EntityCollider(Mesh* mesh, const Material& material, col_type type = SCENARIO, const std::string& name = "") {
		this->type = type;
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
	};
	~EntityCollider() {};
	bool is_dynamic = false;
};


