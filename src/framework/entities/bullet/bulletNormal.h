#pragma once
#ifndef BULLET_NORMAL_H
#define BULLET_NORMAL_H

#include "framework/entities/bullet/bullet.h"

class BulletNormal : public Bullet {

public:
	BulletNormal() {
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->speed = BULLET_SPD;
		this->timer_spawn = Game::instance->time;
	};
	BulletNormal(Mesh* mesh, const Material& material, Vector3 direction, Matrix44 model, float speed = BULLET_SPD, const std::string& name = "", bool fromPlayer = true) : Bullet(fromPlayer) {
		this->mesh = mesh;
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->speed = speed;
		this->direction = direction;
		this->model = model;
		this->timer_spawn = Game::instance->time;
	};
	~BulletNormal() {

	}
	


	void move(Vector3 vec);
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void addInstance(Matrix44 model, float speed) {
		this->models.push_back(model);
		this->speeds.push_back(speed);
	};

	void onMouseWheel(SDL_MouseWheelEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	Vector3 getPosition();

};

#endif