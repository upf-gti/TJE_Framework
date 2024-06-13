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
	virtual void render(void) {};
	virtual void update(double dt) {};

	//some vars
	Entity* root_opaque;
	Entity* root;
	Entity* root_transparent;
	Entity* GUI;

	Enemy* enemy;
	Player* player;

	Shader* hudshader;
	Shader* picshader;
	Shader* squareshader;

	Shader* shader;

	float anxiety;
	float anxiety_dt = 0;

	std::string nextStage;

	Camera* camera; //our global camera
	Camera* camera2D; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	//events
	virtual void onKeyDown(SDL_KeyboardEvent event) {};
	virtual void onKeyUp(SDL_KeyboardEvent event) {};
	virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};
	virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
	virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
	virtual void onGamepadButtonUp(SDL_JoyButtonEvent event) {};



	virtual void handlePlayerHP(Player* p, float hp) {};
	virtual void handleEnemyHP(Enemy* e, float hp) {};

	virtual bool ray_collided(Entity* root, std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO) { return false; };
	virtual COL_TYPE sphere_collided(Entity* root, std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO, bool check = false) { return COL_TYPE::NONE; };

	virtual void resize();
};