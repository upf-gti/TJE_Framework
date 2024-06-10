#pragma once
#include "stage.h"

class Audio;
class RenderToTexture;
class FBO;
class Light;

class GameStage : Stage
{
public:
	float zoom = 5.f;

	GameStage();

	//main functions
	void render(void) override;
	void update(double seconds_elapsed) override;

	Vector3 cam_position;

	Light* mainLight;

	RenderToTexture* renderFBO;
	FBO* shadowMapFBO;

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

	void renderPlain(const Matrix44 model, Mesh* mesh, Material* material);

	bool ray_collided(Entity* root, std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO) override;
	COL_TYPE sphere_collided(Entity* root, std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO, bool check = false) override;
  
	void renderHUD();
 	void renderBar(Vector2 barPosition, Vector2 barSize, float percentage, Vector3 color);

	int shadowMapSize = 1024;
	void generateShadowMaps(Camera* camera);

	static bool compareFunction(const Entity* e1, const Entity* e2);

	bool parseScene(const char* filename);

	void resize() override;
};