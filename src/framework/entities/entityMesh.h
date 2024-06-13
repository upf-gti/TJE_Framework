#pragma once

#include "framework/entities/entity.h"
#include "framework/animation.h"
#include "graphics/material.h"

struct sMeshLOD {
	Mesh* mesh; // .ASE File or .OBJ File
	float distance;
};

enum COL_TYPE : int32 {
	NONE = 0,
	FLOOR = 1,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	ENEMY = 1 << 3,
	PBULLET = 1 << 4,
	EBULLET = 1 << 5,
	BORDER = 1 << 6,
	COLUMN = 1 << 7,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR | BORDER | COLUMN,
	EBCOLS = SCENARIO | PLAYER,
	PBCOLS = SCENARIO | ENEMY,
	ALL = 0xFF
};

class EntityMesh : public Entity {

public:
	COL_TYPE type;
	std::vector<sMeshLOD> mesh_lods;
	// Attributes of the derived class  
	Mesh* mesh = nullptr;

	Vector4 color;
	Animation* anim;
	Animation* last_anim;
	Material material;

	bool isInstanced = false;
	bool isAnimated = false;

	std::vector<Matrix44> models;

	void addInstance(const Matrix44& model) {
		models.push_back(model);
	};

	EntityMesh() { };
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "") {
		this->mesh = mesh;
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
	};
	~EntityMesh() {}
	
	Vector3 getPosition() const { return model.getTranslation(); };
	// Methods overwritten from base class
	void render(Camera* camera);
	void renderWithLights(Camera* camera) override;
	void update(float elapsed_time);
	void addLOD(sMeshLOD mesh_lod);
};

