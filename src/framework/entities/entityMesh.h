#pragma once

#include "framework/entities/entity.h"

#include "graphics/material.h"


class EntityMesh : public Entity {

public:
	enum col_type : int32{
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

	col_type type;

	struct sMeshLOD {
		Mesh* mesh; // .ASE File or .OBJ File
		float distance;
	};
	std::vector<sMeshLOD> mesh_lods;
	// Attributes of the derived class  
	Mesh* mesh = nullptr;

	Vector4 color;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	void addInstance(const Matrix44& model) {
		models.push_back(model);
	};

	EntityMesh() {};
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "") {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
	};
	~EntityMesh() {}
	

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void addLOD(sMeshLOD mesh_lod);
};

