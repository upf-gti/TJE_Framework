/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityCollider.h"

class Enemy;
class Player;

class Stage
{
public:
	//some globals
	long frame;
	float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	float zoom = 5.f;

	Entity* root;
	Enemy* enemy;
	Player* player;

	//some vars
	Camera* camera; //our global camera
	Camera* camera2D; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Stage() {}

	virtual void render() {};
	virtual void update(float seconds_elapsed) {};

	virtual void onKeyDown(SDL_KeyboardEvent event) {};
	virtual void onKeyUp(SDL_KeyboardEvent event) {};
	virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};
	virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
	virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
	virtual void onGamepadButtonUp(SDL_JoyButtonEvent event) {};

	virtual bool ray_collided(std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO) { return false; }
	virtual bool sphere_collided(std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO) { return false; }
};

class SecondStage : Stage
{
public:
	static SecondStage* instance;
	float zoom = 5.f;

	Entity* root;
	Enemy* enemy;
	Player* player;

	SecondStage();

	//main functions
	void render(void);
	void update(double dt);


	//events
	void onKeyDown(SDL_KeyboardEvent event);
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	static bool compareFunction(const Entity* e1, const Entity* e2);

	bool ray_collided(std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO);
	bool sphere_collided(std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO);
};