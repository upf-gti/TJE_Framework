#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "framework/entities/entityMesh.h"
#include "framework/entities/bullet/bulletAuto.h"
#include "framework/entities/bullet/bulletNormal.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "framework/entities/bullet/patterns.h"



#define DEFAULT_SPD 500
#define DEFAULT_MANA 100
#define JUMP_SPD 1000
#define JUMP_HOLDTIME 1.0f
#define GRAVITY 3000
#define MAX_BULLETS 1000
#define DEFAULT_FIRERATE 0.3	// Fire Rate of Autoaim
#define DEFAULT_COST 7			// Mana Cost of Autoaim


class Player : public EntityMesh {

public:
	// Movement
	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f), right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 direction;
	Vector3 box_cam;
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
	
	// Bullets
	enum bullet_type : uint8{
		auto_aim,
		circle,
		shotgun,
		sniper
	};
	bullet_type bt = circle;
	float mana;
	bool autoshoot = false;
	std::vector<Bullet*> bullets;
	uint16 bullet_idx_first = 0;
	uint16 bullet_idx_last = 0;
	uint16 free_bullets = MAX_BULLETS;
	float timer_bullet_general = 0;
	float timer_bullet[4] = {0,0,0,0};
	float shoot_cooldown[4] = { DEFAULT_FIRERATE, 1, 1, 1 }, shoot_cost[4] = { DEFAULT_COST, 30, 60, 80};
	float knockback[4] = { 2, 0, 10, 20 }, knockback_time[4] = { .5, 0, .5, 1 };
	typedef void (*PatternFunc) (Vector3 objective, Vector3 direction, Matrix44 model, std::vector<Bullet*>& bullets, int amount);
	uint16 amount[4] = { 1, 10, 20, 1 };
	PatternFunc patterns[4] = { Patterns::autoAim , Patterns::circle, Patterns::shotgun, Patterns::sniper };

	// TODO: Hitbox stuff 
	bool can_be_hit = true;

	Player() {
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->mana = DEFAULT_MANA;
		// this->speed = 10;
	};
	Player(Mesh* mesh, const Material& material, const std::string& name = "", float speed = 0, float mana = DEFAULT_MANA) {
		this->mesh = mesh;
		material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->mana = mana;
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
	void onKeyDown(SDL_KeyboardEvent event);

	Vector3 getPosition();
	Vector3 getPositionGround();

private:
	void dash(float delta_time, float dash_duration, float invul_duration);
	void jump(float delta_time);
	void shoot(bullet_type bullet_type);
};



#endif