#pragma once
#ifndef BULLET_H
#define BULLET_H

#include "framework/entities/entityCollider.h"
#include "game/game.h"

#define BULLET_SPD 10


// Bullets
enum bullet_type : uint8 {
	auto_aim,
	circle,
	shotgun,
	sniper
};

class Bullet : public EntityCollider {

public:
	// Movement
	Vector3 direction;
	bool fromPlayer;
	float rotation_angle = 0;
	float acceleration = 0;
	float rotation_angle_accel = 0;
	float speed = BULLET_SPD;
	float opacity_dec = 3;
	float damage;
	float time_since_spawn = 0;

	std::vector<float> speeds;
	std::vector<float> accels;
	std::vector<float> angular_speeds;
	std::vector<float> angular_accels;

	void despawnBullet(int i) {
		models.erase((models.begin() + i));
		speeds.erase((speeds.begin() + i));
		accels.erase((accels.begin() + i));
		angular_speeds.erase((angular_speeds.begin() + i));
		angular_accels.erase((angular_accels.begin() + i));
	}

	bool active = true;
	bool to_delete = false;

	// Despawn time
	float timer_spawn = -1;
	
	Bullet() { type = COL_TYPE::PBULLET; damage = 1; };
	Bullet(bool fromPlayer) { type = COL_TYPE::PBULLET; damage = 1; this->fromPlayer = fromPlayer; }
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
		//std::cout << material.color.w;
		if (material.color.w > 0)
			material.color.w -= opacity_dec * delta_time;
		else {
			material.color.w = 0;
			to_delete = true;
		}
	}

};

#endif