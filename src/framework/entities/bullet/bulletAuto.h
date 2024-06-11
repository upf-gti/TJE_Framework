#pragma once
#ifndef BULLET_AUTO_H
#define BULLET_AUTO_H

#include "framework/entities/bullet/bullet.h"


class BulletAuto : public Bullet {

public:
	Vector3 objective;
	

	BulletAuto() {
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->speed = BULLET_SPD;
		this->timer_spawn = Game::instance->time;
	};
	BulletAuto(Mesh* mesh, const Material& material, Vector3 objective, Vector3 direction, Matrix44 model, float speed = BULLET_SPD,  const std::string& name = "", bool fromPlayer = true) : Bullet(fromPlayer) {
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->mesh = mesh;
		this->material = material;
		this->speed = speed;
		this->objective = objective;
		this->direction = direction;
		this->model = model;
		this->timer_spawn = Game::instance->time;
	};
	~BulletAuto() {

	}
	
	void addInstance(Matrix44 model, float speed, float accel = 0, float angular_speed = 0, float angular_accel = 0) {
		this->spawn_time.push_back(Game::instance->time);

		this->models.push_back(model);
		this->speeds.push_back(speed);
		this->accels.push_back(accel);
		this->angular_speeds.push_back(angular_speed);
		this->angular_accels.push_back(angular_accel);
	};

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