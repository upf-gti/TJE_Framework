#pragma once
#include "stage.h"

class Audio;
class RenderToTexture;
class FBO;
class Light;

class GameStage : Stage
{
public:
	static GameStage* instance;
	float zoom = 5.f;

	GameStage();

	//main functions
	void render(void) override;
	void update(double seconds_elapsed) override;

	float trees_shoot = 0;
	float interval = 10;

	float columns_shoot = 0;
	float columns_interval = 5;

	Vector3 cam_position;

	Light* mainLight;
	Light* centerLight;

	std::vector<Light*> lights;

	RenderToTexture* renderFBO;
	FBO* shadowMapFBO;


	float enoughmanatimer = 0;
	float enoughmana;

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

	bool ray_collided(Entity* root, std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space = false, COL_TYPE collision_type = SCENARIO) override;
	COL_TYPE sphere_collided(Entity* root, std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type = SCENARIO, bool check = false) override;
  
	void renderHUD();
 	void renderBar(Vector2 barPosition, Vector2 barSize, float percentage, Vector3 color, float decrease = 0);

 	void renderSquare(Vector2 barPosition, Vector2 barSize, float percentage, Vector4 color);
	void renderPic(Vector2 position, Vector2 size, Texture* diffuse);


	void generateShadowMaps(Camera* camera);

	static void shadowToShader(Light* light, int& shadowMapPos, Shader* shader);
	static void lightToShader(Light* light, Shader* shader);

	static bool compareFunction(const Entity* e1, const Entity* e2);

	bool parseScene(const char* filename);

	void resize() override;
};