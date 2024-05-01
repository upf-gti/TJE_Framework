#pragma once
#ifndef BULLET_H
#define BULLET_H

#include "framework/entities/entity.h"


#include "game/game.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/material.h"

#define BULLET_SPD 1000


class Bullet : public Entity {

public:

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Vector4 color;
	// float speed;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	// Movement
	Vector3 direction;
	bool has_objective = false;
	Vector3 objective;
	float rotation_angle = 0;
	float speed;

	// Despawn time
	float timer_spawn;
	

	void addInstance(const Matrix44& model);

	Bullet() {
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->speed = BULLET_SPD;
		this->timer_spawn = Game::instance->time;
	};
	Bullet(Mesh* mesh, const Material& material, const std::string& name = "", float speed = BULLET_SPD) {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->speed = speed;
		this->timer_spawn = Game::instance->time;
	};
	~Bullet() {

	}
	
	void move(Vector3 vec);
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void onMouseWheel(SDL_MouseWheelEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	Vector3 getPosition();

};

#endif