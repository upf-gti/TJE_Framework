/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityCollider.h"

class Stage
{
public:
	static Stage* instance;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	float zoom = 5.f;

	Entity* root;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Stage();

	//main functions
	void render( void );
	void update( double dt );


	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	bool ray_collided(std::vector<EntityCollider::sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, EntityCollider::col_type collision_type = EntityCollider::SCENARIO);
	bool sphere_collided(std::vector<EntityCollider::sCollisionData>& collisions, Vector3 position, float radius, EntityCollider::col_type collision_type = EntityCollider::SCENARIO);
	static bool comparefunction(const Entity* e1, const Entity* e2);
private:
	
};