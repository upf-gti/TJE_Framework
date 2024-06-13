#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "framework/entities/entityCollider.h"
#include "framework/entities/bullet/bulletAuto.h"
#include "framework/entities/bullet/bulletNormal.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/stage.h"
#include "framework/entities/bullet/patterns.h"



#define DEFAULT_SPD 2.5
#define DEFAULT_MANA 200
#define JUMP_SPD 10
#define JUMP_HOLDTIME 1.0f
#define GRAVITY 40
#define MAX_BULLETS 1000
#define DEFAULT_FIRERATE 0.3	// Fire Rate of Autoaim
#define DEFAULT_COST 7			// Mana Cost of Autoaim
#define HITBOX_RAD 0.9
#define PLAYER_HEIGHT 0.6


class Player : public EntityCollider {

public:
	float maxHP = 10;
	float currHP = maxHP;
	// Movement
	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f), right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 direction;
	Vector3 box_cam;
	Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
	float stop_duration = 0.25;
	float m_spd = DEFAULT_SPD;
	float v_spd = 0;

	bool targetable = true;
	float startHit = 0;

	// Dashing
	bool dashing = false;
	float timer_dash;
	float dashInvulnerabilityTimer = 0;

	// Jumping
	bool grounded = true;
	bool touching_ground = false;
	bool jumping = false;
	bool colliding = false;
	float timer_jump = 0;
	float player_height = PLAYER_HEIGHT;

	bullet_type bt = circle;
	float mana;
	bool autoshoot = false;
	std::vector<Bullet*> bullets;
	uint16 bullet_idx_first = 0;
	uint16 bullet_idx_last = 0;
	uint16 free_bullets = MAX_BULLETS;
	float timer_bullet_general = 0;
	float timer_bullet[4] = { 0,0,0,0 }, timer_charge[4] = { 0,0,0,0 };
	float shoot_cooldown[4] = { DEFAULT_FIRERATE, .3, .5, 1 }, shoot_cost[4] = { DEFAULT_COST, 45, 105, 160 };
	float knockback[4] = { 2, 0, 10, 10 }, knockback_time[4] = { .5, 0, .5, 1 };
	float charge_cooldown[4] = { 0,0,0,1 }; bool charging = false;
	Shader* bullet_shaders[4]; Texture* bullet_textures[4]; Mesh* bullet_meshes[4];
	bool canshoot = true;
	uint16 amount[4] = { 1, 10, 20, 1 };
	PatternFunc patterns[4] = { (PatternFunc) Patterns::autoAim , (PatternFunc) Patterns::circle, (PatternFunc) Patterns::shotgun, (PatternFunc) Patterns::sniper };
	Material charge_mat;
	Mesh* charge_mesh;
	Matrix44 charge_model;

	EntityMesh dir;
	EntityMesh vec;

	BulletNormal bullets_normal;
	BulletAuto bullets_auto;

	float ground_below_y = 10000;
	float ground_y = 10000;
	Vector3 ground_normal;

	Shader* flat_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");

	Mesh* hitbox_mesh = Mesh::Get("data/meshes/sphere.obj");
	Mesh* shadow_mesh = Mesh::Get("data/meshes/shadow.obj");

	enum anim_type : uint8 {
		IDLE, WALKING, DASH
	};
	anim_type current_animation = IDLE;
	anim_type last_animation = IDLE;
	Animation* animation_pool[3];
	float timer_anim = 0;

	void sphere_bullet_collision(Vector3 position, float radius);

	HCHANNEL charge_channel;

	// TODO: Hitbox stuff 
	bool can_be_hit = true;

	void loadAnims() {
		animation_pool[IDLE] = Animation::Get("data/anims/char_idle.skanim");
		animation_pool[WALKING] = Animation::Get("data/anims/char_walk.skanim");
		animation_pool[DASH] = Animation::Get("data/anims/char_run.skanim");
		anim = animation_pool[IDLE];
	}

	Player() {
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->mana = DEFAULT_MANA;
		this->type = PLAYER;
		loadTextures();
		loadAnims();

		dir.mesh = Mesh::Get("data/meshes/directon.obj");
		dir.material.diffuse = Texture::Get("data/meshes/directon.mtl");
		dir.material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
		dir.material.color = Vector4(1);

		vec.mesh = Mesh::Get("data/meshes/vec.obj");
		vec.material.diffuse = Texture::Get("data/meshes/vec.mtl");
		vec.material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		vec.material.color = Vector4(0.5,0.5,1,0.2);
	};
	Player(Mesh* mesh, const Material& material, const std::string& name = "", float speed = 0, float mana = DEFAULT_MANA) {
		this->mesh = mesh;
		if (!mesh) std::cout << "NO MESH";
		//material.shader == nullptr ? std::cout << "NULL SHADER" : std::cout << "GOOD SHADER";
		this->material = material;
		this->mana = mana;
		this->type = PLAYER;
		loadTextures();
		loadAnims();

	};
	~Player() {

	}
	
	void move(const Vector3& vec);
	// Methods overwritten from base class
	void render(Camera* camera);	
	void renderWithLights(Camera* camera) override;
	void update(float elapsed_time);
	float updateSubframe(float elapsed_time);

	void onMouseWheel(SDL_MouseWheelEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onKeyUp(SDL_KeyboardEvent event);
	void onKeyDown(SDL_KeyboardEvent event);

	Vector3 getPosition() const { return model.getTranslation(); };
	Vector3 getPositionGround() const { Vector3 res = getPosition(); res.y = 0; return res; }
	Vector3 getFront() { return model.frontVector(); }
	Vector3 getRight() { return model.rightVector(); }

private:
	void loadTextures() {
		bullets_normal.isInstanced = true;
		bullets_normal.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_normal.material.diffuse = Texture::Get("data/meshes/bullet.mtl");
		bullets_normal.mesh = Mesh::Get("data/meshes/bullet.obj");
		bullets_normal.material.color = Vector4(51/255, 1, 1, 1);
		bullets_normal.damage = 0.5;

		bullets_auto.isInstanced = true;
		bullets_auto.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_auto.material.diffuse = Texture::Get("data/meshes/bullet.mtl");
		bullets_auto.mesh = Mesh::Get("data/meshes/bullet.obj");
		bullets_auto.material.color = Vector4(0.0, 68/255, 1, 1);
		bullets_auto.damage = 0.25;

		charge_mat.color = Vector4(1, 1, 1, 1);
		charge_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		charge_mat.diffuse = Texture::Get("data/meshes/charge.mtl");
		charge_mesh = Mesh::Get("data/meshes/charge.obj");

		Shader* s = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		bullet_shaders[0] = bullet_shaders[1] = bullet_shaders[2] = bullet_shaders[3] = s;
		Texture* t1 = Texture::Get("data/meshes/bullet.mtl");
		Texture* t2 = Texture::Get("data/meshes/sniper.mtl");
		bullet_textures[0] = bullet_textures[1] = bullet_textures[2] = t1;
		bullet_textures[3] = t2;
		Mesh* m1 = Mesh::Get("data/meshes/bullet.obj");
		Mesh* m2 = Mesh::Get("data/meshes/sniper.obj");
		bullet_meshes[0] = bullet_meshes[1] = bullet_meshes[2] = m1;
		bullet_meshes[3] = m2;
	}
	void dash(float delta_time, float dash_duration, float invul_duration);
	void jump(float delta_time);
	void shootCharge(bullet_type bullet_type);
	void shoot(bullet_type bullet_type);
	void showHitbox(Camera* camera);
	void chargingShot(Camera* camera);
};

#endif