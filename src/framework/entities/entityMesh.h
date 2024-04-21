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
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "") {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
	};
	~EntityMesh() {}
	

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
};

