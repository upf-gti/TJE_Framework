#pragma once
#ifndef BULLET_H
#define BULLET_H

#include "framework/entities/entityMesh.h"
#include "game/game.h"

#define BULLET_SPD 10


class Bullet : public EntityMesh {

public:
	// Movement
	Vector3 direction;
	float rotation_angle = 0;
	float acceleration = 0;
	float rotation_angle_accel = 0;
	float speed = BULLET_SPD;
	float opacity_dec = 0;

	// Despawn time
	float timer_spawn = -1;
	

	Bullet() {};
	~Bullet() {};
	
	void move(Vector3 vec) {};
	// Methods overwritten from base class
	void render(Camera* camera) {};
	void update(float elapsed_time) {};

	void onMouseWheel(SDL_MouseWheelEvent event) {};
	void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	void onKeyUp(SDL_KeyboardEvent event) {};

	Vector3 getPosition() {};

};

#endif