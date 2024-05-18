#pragma once

#include "framework/entities/entityMesh.h"

#include "graphics/material.h"


class EntityCollider : public EntityMesh {

public:

	struct sCollisionData {
		Vector3 colPoint;
		Vector3 colNormal;
	};


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


