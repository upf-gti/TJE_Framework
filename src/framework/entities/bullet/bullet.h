#pragma once
#ifndef BULLET_H
#define BULLET_H

#include "framework/entities/entityCollider.h"
#include "game/game.h"

#define BULLET_SPD 10


class Bullet : public EntityCollider {

public:
	// Movement
	Vector3 direction;
	float rotation_angle = 0;
	float acceleration = 0;
	float rotation_angle_accel = 0;
	float speed = BULLET_SPD;
	float opacity_dec = 3;
	float damage;

	bool active = true;
	bool to_delete = false;

	// Despawn time
	float timer_spawn = -1;
	

	Bullet() { type = COL_TYPE::BULLET; damage = 1; };
	~Bullet() {};
	
	void move(Vector3 vec) {};
	// Methods overwritten from base class
	void render(Camera* camera) {};
	void update(float elapsed_time) {};

	void onMouseWheel(SDL_MouseWheelEvent event) {};
	void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	void onKeyUp(SDL_KeyboardEvent event) {};

	Vector3 getPosition() {};

protected:
	void despawning(float delta_time) {
		std::cout << material.color.w;
		if (material.color.w > 0)
			material.color.w -= opacity_dec * delta_time;
		else {
			material.color.w = 0;
			to_delete = true;
		}
	}

};

#endif