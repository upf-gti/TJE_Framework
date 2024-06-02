#pragma once
#include "stage.h"

class GameStage : Stage
{
public:
	float zoom = 5.f;

	GameStage();

	//main functions
	void render(void) override;
	void update(double seconds_elapsed) override;

	Vector3 cam_position;

	//events
	void onKeyDown(SDL_KeyboardEvent event) override;
	void onKeyUp(SDL_KeyboardEvent event) override;
	void onMouseButtonDown(SDL_MouseButtonEvent event) override;
	void onMouseButtonUp(SDL_MouseButtonEvent event) override;
	void onMouseWheel(SDL_MouseWheelEvent event) override;
	void onGamepadButtonDown(SDL_JoyButtonEvent event) override;
	void onGamepadButtonUp(SDL_JoyButtonEvent event) override;

	void handlePlayerHP(Player* p, float hp) override;
	void handleEnemyHP(Enemy* e, float hp) override;

	bool ray_collided(std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO) override;
	COL_TYPE sphere_collided(std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO, bool check = false) override;
	static bool compareFunction(const Entity* e1, const Entity* e2);

	bool parseScene(const char* filename);

};