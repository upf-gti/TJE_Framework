#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "framework/entities/entityCollider.h"
#include "framework/entities/bullet/patterns.h"
#define HITBOX_RAD 0.1
#define PLAYER_HEIGHT 0.6

class Bullet;

class Enemy : public EntityCollider {

public:
	Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP);

	void render(Camera* camera);
	void renderWithLights(Camera* camera) { render(camera); }

	void update(float time_elapsed);
	Vector3 updateSubframe(float elapsed_time);

	bool touching_ground = false;
	bool colliding = false;
	bool grounded;
	float ground_below_y = 10000;
	float ground_y = 10000;
	Vector3 ground_normal = Vector3(0.0f);
	Vector3 direction = Vector3(0.0f);

	BulletNormal bullets_normal;
	BulletNormal bullets_ball;
	BulletResize bullets_giantball;
	BulletNormal bullets_smallball;

	int maxHP;
	int currHP;
	float m_spd;
	float v_spd;

	bool moving;
	float startMoving;
	float startFiring;
	float bulletCD = -1;
	float burstCD = -1;
	int burstCount = 0;

	Matrix44 patternTarget;

	bool looking_at_player = true;

	Mesh* hitbox_mesh = Mesh::Get("data/meshes/sphere.obj");
	Shader* flat_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");

	Mesh* shadow_mesh = Mesh::Get("data/meshes/shadow.obj");

	enum pattern {
		SWIRL,
		FLOWER,
		HORIZONTAL,
		SPIRAL,
		SHOTGUN,
		RINGS,
		TRAP,
		REV,
		WAVY,
		WAVY2,
		SPIRALBURST,
		SUN,
		RINGSHOT
	};

	std::vector<Bullet*> bullets;
	Shader* bullet_shaders[4]; Texture* bullet_textures[4]; Mesh* bullet_meshes[4];
	pattern current_pattern = SWIRL;
	bool canshoot = true;
	uint16 amount[4] = {6, 10, 20, 1 };

	Vector3 getPosition() const { return model.getTranslation(); };
	Vector3 getHitboxPosition() const { return getPosition() + Vector3(0, 1.2, 0); };
	Vector3 getPositionGround() const { Vector3 res = getPosition(); res.y = 0; return res; }
	Vector3 getFront() { return model.frontVector(); }
	Vector3 getRight() { return model.rightVector(); }

	void randomPattern();

	void showHitbox(Camera* camera);
	void sphere_bullet_collision(Vector3 position, float radius);

	void loadTextures() {
		bullets_normal.isInstanced = true;
		bullets_normal.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_normal.material.diffuse = Texture::Get("data/meshes/bullet.mtl");
		bullets_normal.fromPlayer = false;
		bullets_normal.mesh = Mesh::Get("data/meshes/bullet.obj");
		bullets_normal.material.color = Vector4(1, 0.0, 0.0, 1);
		bullets_ball.damage = 5;

		bullets_smallball.isInstanced = true;
		bullets_smallball.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_smallball.material.diffuse = Texture::Get("data/meshes/bulletballsmall.mtl");
		bullets_smallball.fromPlayer = false;
		bullets_smallball.mesh = Mesh::Get("data/meshes/bulletballsmall.obj");
		bullets_smallball.material.color = Vector4(1, 0.0, 0.5, 1);
		bullets_ball.damage = 5;

		bullets_ball.isInstanced = true;
		bullets_ball.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_ball.material.diffuse = Texture::Get("data/meshes/bulletball.mtl");
		bullets_ball.material.color = Vector4(1, 0, 1, 1);
		bullets_ball.fromPlayer = false;
		bullets_ball.mesh = Mesh::Get("data/meshes/bulletball.obj");
		bullets_ball.damage = 8;

		bullets_giantball.isInstanced = true;
		bullets_giantball.material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
		bullets_giantball.material.diffuse = Texture::Get("data/meshes/bulletballgiant.mtl");
		bullets_giantball.material.color = Vector4(0.9, 0.4, 0, 1);
		bullets_giantball.fromPlayer = false;
		bullets_giantball.mesh = Mesh::Get("data/meshes/bulletballgiant.obj");
		bullets_giantball.damage = 20;

		Shader* s = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		bullet_shaders[0] = s;
		Texture* t1 = Texture::Get("data/meshes/bullet.mtl");
		bullet_textures[0] = t1;
		Mesh* m1 = Mesh::Get("data/meshes/bullet.obj");
		bullet_meshes[0] = m1;

		anim = Animation::Get("data/anims/maolixi/maolixi_idle.skanim");
	}
};

#endif