#pragma once

#include "framework/entities/entity.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/material.h"


class Player : public Entity {

public:

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Vector4 color;
	// float speed;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	Vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
	Vector3 right = Vector3(-1.0f, 0.0f, 0.0f);

	void addInstance(const Matrix44& model);

	Player() {
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		// this->speed = 10;
	};
	Player(Mesh* mesh, const Material& material, const std::string& name = "", float speed = 10) {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		// this->speed = speed;
	};
	~Player() {

	}
	
	void move(Vector3 vec);
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void onMouseWheel(SDL_MouseWheelEvent event);
};

