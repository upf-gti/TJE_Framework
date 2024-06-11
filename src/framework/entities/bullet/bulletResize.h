#pragma once
#ifndef BULLET_RESIZE_H
#define BULLET_RESIZE_H

#include "framework/entities/bullet/bullet.h"

class BulletResize : public Bullet {

public:
	BulletResize() {
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->speed = BULLET_SPD;
		this->timer_spawn = Game::instance->time;
	};
	BulletResize(Mesh* mesh, const Material& material, Vector3 direction, Matrix44 model, float speed = BULLET_SPD, const std::string& name = "", bool fromPlayer = true) : Bullet(fromPlayer) {
		this->mesh = mesh;
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->speed = speed;
		this->direction = direction;
		this->model = model;
		this->timer_spawn = Game::instance->time;
	};
	~BulletResize() {

	}

	std::vector<float> sizes;
	std::vector<float> sizes_spd;
	std::vector<float> sizes_accel;
	std::vector<Vector2> sizes_limits;
	std::vector<Vector2> sizes_spd_limits;

	std::vector<Matrix44> __models;

	void move(Vector3 vec);
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void addInstance(Matrix44 model, float speed, float size, float size_spd, float size_accel, Vector2 size_limits, Vector2 size_spd_limits, float accel = 0, float angular_speed = 0, float angular_accel = 0) {
		this->models.push_back(model);
		this->__models.push_back(model);

		this->sizes.push_back(size);
		this->sizes_spd.push_back(size_spd);
		this->sizes_accel.push_back(size_accel);
		this->sizes_limits.push_back(size_limits);
		this->sizes_spd_limits.push_back(size_spd_limits);

		this->speeds.push_back(speed);
		this->accels.push_back(accel);
		this->angular_speeds.push_back(angular_speed);
		this->angular_accels.push_back(angular_accel);
	};

	void despawnBullet(int i) {
		models.erase((models.begin() + i));
		__models.erase((__models.begin() + i));
		speeds.erase((speeds.begin() + i));
		accels.erase((accels.begin() + i));
		angular_speeds.erase((angular_speeds.begin() + i));
		angular_accels.erase((angular_accels.begin() + i));

		sizes.erase((sizes.begin() + i));
		sizes_spd.erase((sizes_spd.begin() + i));
		sizes_accel.erase((sizes_accel.begin() + i));
		sizes_limits.erase((sizes_limits.begin() + i));
		sizes_spd_limits.erase((sizes_spd_limits.begin() + i));
	}


	void onMouseWheel(SDL_MouseWheelEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	Vector3 getPosition();

};

#endif