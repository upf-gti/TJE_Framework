#pragma once

#include "framework/entities/entity.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/material.h"

#define DEFAULT_SPD 200
#define JUMP_SPD 1000
#define JUMP_HOLDTIME 1.0f
#define GRAVITY 3000

class Player : public Entity {

public:

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Vector4 color;
	// float speed;

	Material material;

	bool isInstanced = false;
	std::vector<Matrix44> models;

	// Movement
	Vector3 forward = Vector3(0.0f, 0.0f, -1.0f), right = Vector3(-1.0f, 0.0f, 0.0f);
	Vector3 direction;
	float stop_duration = 0.25;
	float m_spd = DEFAULT_SPD;
	float v_spd = 0;
	// Dashing
	bool dashing = false;
	float timer_dash;
	// Jumping
	bool grounded = true;
	bool jumping = false;
	float timer_jump;
	
	

	// TODO: Hitbox stuff 
	bool can_be_hit = true;

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
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

private:
	void dash(float delta_time, float dash_duration, float invul_duration);
	void jump(float delta_time);
};

