#pragma once
#ifndef BULLET_SNIPER_H
#define BULLET_SNIPER_H

#include "framework/entities/bullet/bullet.h"


class BulletSniper : public Bullet {

public:
	float scale = 0.1;
	Mesh* hitbox_mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 model_base;
	BulletSniper() {
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->speed = BULLET_SPD;
		this->timer_spawn = Game::instance->time;
	};
	BulletSniper(Mesh* mesh, const Material& material, Vector3 direction, Matrix44 model, float speed = BULLET_SPD,  const std::string& name = "") : Bullet() {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->speed = speed;
		this->direction = direction;
		this->model = model;
		this->model_base = model;
		this->timer_spawn = Game::instance->time;
	};
	~BulletSniper() {

	}
	
	void move(Vector3 vec);
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void onMouseWheel(SDL_MouseWheelEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	Vector3 getPosition();
private:
	void drawHitBox(Camera* camera);
};

#endif