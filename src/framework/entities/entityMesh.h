#pragma once

#include "framework/entities/entity.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/material.h"


class EntityMesh : public Entity {

public:

	// Attributes of the derived class  
	Mesh* mesh = nullptr;

	Vector4 color;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	void addInstance(const Matrix44& model);

	EntityMesh() {};
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "") {};
	~EntityMesh() {}
	

	// Methods overwritten from base class
	void render();
	void update(float elapsed_time);
};

