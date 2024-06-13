#include "GameStage.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "graphics/material.h"
#include "graphics/render_to_texture.h"
#include "framework/entities/enemy.h"
#include "framework/entities/player.h"
#include "framework/entities/entityUI.h"
#include "StageManager.h"
#include "framework/audio.h"
#include "framework/entities/Light.h"
#include "framework/entities/bullet/patterns.h"

#include <fstream>
#include <cmath>
#include <unordered_map>
#include <bitset>
#include <algorithm>
#include <vector>


//some globals
Mesh* mesh = NULL;
Mesh* cube = NULL;
Texture* texture = NULL;
float angle = 0;
float mouse_speed = 100.0f;

float camera_angle_x = 0;
float camera_angle_y = 100;

Player* player = NULL;
Player* e2 = NULL;
Enemy* enemy = NULL;
Matrix44 camera_support;

Mesh* quad;

Texture* cubemap = new Texture();

Shader* image = NULL;
Texture* sus;
Texture* slot1;
Texture* slot2;
Texture* slot3;
Texture* slotauto;
Texture* selected;
Texture* shoot_tuto;

EntityUI amogus;

GameStage* GameStage::instance = NULL;

// Cosas nuevas que he añadido


static void renderSkybox(Texture* cubemap)
{
	Camera* camera = Camera::current;

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skybox.fs");
	if (!shader)
		return;
	shader->enable();

	Matrix44 m;
	m.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	m.scale(10, 10, 10);
	shader->setUniform("u_model", m);

	shader->setUniform("u_camera_pos", camera->eye);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", cubemap, 0);
	cube->render(GL_TRIANGLES);
	shader->disable();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
}

bool GameStage::parseScene(const char* filename)
{
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	std::unordered_map<std::string, sRenderData > meshes_to_load;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		if (mesh_name[0] == '#')
			continue;

		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, nothing to do here
		if (render_data.models.empty())
			continue;


		Material mat = render_data.material;
		EntityCollider* new_entity = nullptr;

		size_t tag = data.first.find("@wall");

		if (data.first.find("@wall") != std::string::npos) {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = WALL;
			std::cout << "WALL: " << (FLOOR & WALL);
			std::cout << " This is a wall! ";
			// Create a different type of entity
			// new_entity = new ...
		}
		else if (data.first.find("@border") != std::string::npos) {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = BORDER;
			std::cout << "BORDER: " << (BORDER & FLOOR);
			std::cout << " This is a border! ";
		}
		else if (data.first.find("@column") != std::string::npos) {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = COLUMN;
			//std::cout << "BORDER: " << (BORDER & FLOOR);
			std::cout << " This is a COLUMN! ";
		}
		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = FLOOR;
			std::cout << " This is floor! ";
		}
		std::cout << std::endl << "Tag: " << tag << std::endl;
		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
			if (!new_entity->material.shader) new_entity->material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texturepixel.fs");
			std::cout << "Instanced";
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
			if (!new_entity->material.shader) new_entity->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturepixel.fs");
			std::cout << "Normal";
		}

		std::cout << " " << &new_entity->material.shader << std::endl;

		if (data.first.find("@wall") != std::string::npos || data.first.find("@column") != std::string::npos) {
			root_transparent->addChild(new_entity);
			std::cout << " This is a Transparent element";
		}
		else {
			root_opaque->addChild(new_entity);
			std::cout << " This is a Opaque element";
		}


	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void GameStage::handlePlayerHP(Player* p, float hp) {
	p->currHP = clamp(p->currHP + hp, -1000000, p->maxHP);
}
void GameStage::handleEnemyHP(Enemy* e, float hp) {
	e->currHP = clamp(e->currHP + hp, -1000000, e->maxHP);
}

bool GameStage::ray_collided(Entity* root, std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space, COL_TYPE collision_type) {
	for (int i = 0; i < root->children.size(); ++i) {
		Entity* e = root->children[i];
		ray_collided(e, ray_collisions, position, direction, dist, in_object_space, collision_type);
		EntityMesh* ee = dynamic_cast <EntityMesh*> (e);
		if (!ee) continue; 
		if ((ee->type & collision_type) == 0) continue;
		//std::cout << " Frame:" << Game::instance->time << " Type: " << ee->type << " Requested type: " << collision_type << " Equal? " << (ee->type & collision_type) << std::endl;
		sCollisionData data;
		if (ee->isInstanced) {
			for (Matrix44& instanced_model : ee->models) {
				if (ee->mesh->testRayCollision(
					instanced_model,
					position,
					direction,
					data.colPoint,
					data.colNormal,
					dist,
					in_object_space
				)) {
					ray_collisions.push_back(data);
				}
			}
		}
		else {
			if (ee->mesh->testRayCollision(
				ee->model,
				position,
				direction,
				data.colPoint,
				data.colNormal,
				dist,
				in_object_space
			)) {
				ray_collisions.push_back(data);
			}
		}
	}
	return (!ray_collisions.empty());
}


COL_TYPE GameStage::sphere_collided(Entity* root, std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type, bool check) {
	int return_val = COL_TYPE::NONE;
	for (int i = 0; i < root->children.size(); ++i) {
		Entity* e = root->children[i];
		return_val |= sphere_collided(e, collisions, position, radius, collision_type, check);
		EntityMesh* ee = dynamic_cast <EntityMesh*> (e);
		if (!ee) {
			continue;
		}
		if (!(ee->type & collision_type)) continue;

		sCollisionData data;

		if (ee->isInstanced) {
			for (Matrix44& instanced_model : ee->models) {
				if (ee->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal)) {
					collisions.push_back(data);
					return_val = return_val | ee->type;
				}
			}
		}
		else {
			if (ee->mesh->testSphereCollision(ee->model, position, radius, data.colPoint, data.colNormal)) {
				collisions.push_back(data);
				return_val = return_val | ee->type;
			}
		}
	}
	return (COL_TYPE)return_val;
	// return !collisions.empty();
}



GameStage::GameStage()
{
	GameStage::instance = this;
	mouse_locked = true;
	SDL_ShowCursor(!mouse_locked);
	SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));

    nextStage = "EndStage";

	player = new Player();
	Material* mat = new Material();
	mat->color = Vector4(1, 1, 1, 1);

	mat->shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	mat->diffuse = Texture::Get("data/meshes/asian.png");
	player->mesh = Mesh::Get("data/meshes/char.MESH");


	hudshader = Shader::Get("data/shaders/hud.vs", "data/shaders/hud.fs");
	picshader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	squareshader = Shader::Get("data/shaders/hud.vs", "data/shaders/square.fs");

	slot1 = Texture::Get("data/textures/simple.PNG");
	slot2 = Texture::Get("data/textures/shotgun.PNG");
	slot3 = Texture::Get("data/textures/lazer.png");
	slotauto = Texture::Get("data/textures/auto.PNG");
	selected = Texture::Get("data/textures/selectslot.PNG");
	shoot_tuto = Texture::Get("data/textures/q.PNG");

	player->material = *mat;
	player->isAnimated = true;
	e2 = new Player();
	e2->model.setTranslation(Vector3(10, 0, 5));
	player->model.setTranslation(Vector3(1, 0, 1));
	player->box_cam = Vector3(0, 0, 10);
	// AAA

	Material* mat2 = new Material();
	mat2->color = Vector4(1, 1, 1, 1);
	mat2->shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	mat2->diffuse = Texture::Get("data/meshes/maolixi.png");
	enemy = new Enemy(Mesh::Get("data/meshes/maolixi.MESH"), *mat2, "Francisco", true, 1);
	enemy->model.setTranslation(Vector3(0, 0, 0));
	this->enemy = enemy;
	this->player = player;

	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	// Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	// Hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	root = new Entity();
	root_transparent = new Entity();
	root_opaque = new Entity();
	root->addChild(root_transparent);
	root->addChild(root_opaque);
	parseScene("data/myscene.scene");

	cubemap->loadCubemap("landscape", {
		"data/textures/skybox/right.png",
		"data/textures/skybox/left.png",
		"data/textures/skybox/bottom.png",
		"data/textures/skybox/top.png",
		"data/textures/skybox/front.png",
		"data/textures/skybox/back.png"
		});
	cube = new Mesh();
	cube->createCube();


	player->type = PLAYER;



	anxiety = 30;

	if (!Audio::Init()) std::cout << "Audio not initialized correctly\n";
	Audio::Get("data/audio/whip.wav");
	Audio::Get("data/audio/incorrect.wav");
	Audio::Get("data/audio/charge_lazer.wav");
	Audio::Get("data/audio/lazer.wav");
	Audio::Get("data/audio/shotgun.wav");
	Audio::Get("data/audio/shoot.wav");
	Audio::Get("data/audio/shoot_player.wav");
	Audio::Get("data/audio/h1_0.mp3");
	Audio::Get("data/audio/h1_1.mp3");
	Audio::Get("data/audio/h1_2.mp3");
	Audio::Get("data/audio/h2_0.mp3");
	Audio::Get("data/audio/h2_1.mp3");
	Audio::Get("data/audio/h3_0.mp3");
	Audio::Get("data/audio/h3_1.mp3");
	Audio::Get("data/audio/jump.wav");
	Audio::Get("data/audio/dash_0.wav");
	Audio::Get("data/audio/bgm.mp3", BASS_SAMPLE_LOOP);

	

	renderFBO = NULL;
	
	mainLight = new Light(eLightType::DIRECTIONAL);
	mainLight->model.setTranslation(0, 20, 10);
	centerLight = new Light(eLightType::POINTLIGHT);
	centerLight->model.setTranslation(0, 0, 0);

	lights.push_back(mainLight); lights.push_back(centerLight);
}


void GameStage::renderHUD()
{
	camera2D->enable();
	float gameWidth = Game::instance->window_width, gameHeight = Game::instance->window_height;
	float time = Game::instance->time;
	//Render HP bar
	Vector2 barPosition = Vector2(30 + gameWidth / 4.0, gameHeight - 40 - 15);
	Vector2 barSize = Vector2(gameWidth/2.0f, 30);

	renderBar(barPosition, barSize, anxiety/200.f, Vector3(clamp(255 - anxiety, 0, 255),clamp(anxiety, 0, 255),2)/255.0f, anxiety_dt/200.f);

	barPosition = Vector2(30 + gameWidth * 0.15f, gameHeight - 55 - 30);
	barSize = Vector2(gameWidth*0.3f, 20);

	renderBar(barPosition, barSize, player->mana/300.0f, Vector3(0.3, 0.1, 0.8));


	barPosition = Vector2(45 + 35, gameHeight - 55 - 30 - 70);
	barSize = Vector2(70, 70);

	renderPic(barPosition, barSize, slot1);

	barPosition = Vector2(45 + 35, gameHeight - 55 - 30 - 140);
	barSize = Vector2(70, 70);

	renderPic(barPosition, barSize, slot2);

	barPosition = Vector2(45 + 35, gameHeight - 55 - 30 - 210);
	barSize = Vector2(70, 70);

	renderPic(barPosition, barSize, slot3);

	bullet_type bt = player->bt;
	barPosition = Vector2(45 + 35, gameHeight - 55 - 30 - 70 - 70 * (bt - 1));
	barSize = Vector2(70, 70);

	bool enough_mana = (player->mana > player->shoot_cost[bt]);
	if (enough_mana && !enoughmana) {
		enoughmanatimer = time;
		enoughmana = true;
	}
	else if (!enough_mana) enoughmana = false;
	renderSquare(barPosition, barSize, (time - player->timer_bullet[bt]) / player->shoot_cooldown[bt], Vector4(0, enough_mana + (!enough_mana * 0.7), !enough_mana, 0.3));
	renderPic(barPosition, barSize, selected);
	if (((int)(time - enoughmanatimer) % 2) && enough_mana && enoughmanatimer + 5 < time && player->timer_bullet[bt] + 5 < time) renderPic(barPosition, barSize, shoot_tuto);


	barPosition = Vector2(45 + 35, gameHeight - 55 - 30 - 210 - 80);
	barSize = Vector2(70, 70);

	renderPic(barPosition, barSize, slotauto);

	enough_mana = (player->mana > player->shoot_cost[0]);
	if (player->autoshoot) {
		renderSquare(barPosition, barSize, (time - player->timer_bullet[0]) / player->shoot_cooldown[0], Vector4(enough_mana, (!enough_mana * 0.7), !enough_mana, 0.3));
		renderPic(barPosition, barSize, selected);
	}

}

// Assuming you have a function to loadTGA that reads the TGA data
void GameStage::renderBar(Vector2 barPosition, Vector2 barSize, float percentage, Vector3 color, float decrease)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Mesh innerQuad;
	Mesh outerQuad;
	shader = hudshader;
	shader->enable();

	//Creation of the second quad. This one contains the life information. 
	outerQuad.createQuad(barPosition.x, barPosition.y, barSize.x, barSize.y, true);
	shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	shader->setUniform("u_color", Vector3(32.0f/255.0f));
	shader->setUniform("u_percentage", 1.0f);
	shader->setUniform("u_decrease", 0.0f);

	outerQuad.render(GL_TRIANGLES);

	innerQuad.createQuad(barPosition.x, barPosition.y, barSize.x-10, barSize.y-10, true);
	shader->setUniform("u_color", color);
	shader->setUniform("u_percentage", percentage);
	shader->setUniform("u_decrease", decrease);
	shader->setUniform("u_texture_option", 0.0f);

	innerQuad.render(GL_TRIANGLES);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void GameStage::renderSquare(Vector2 barPosition, Vector2 barSize, float percentage, Vector4 color)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Mesh square;
	shader = squareshader;
	shader->enable();

	//Creation of the second quad. This one contains the life information. 
	square.createQuad(barPosition.x, barPosition.y, barSize.x, barSize.y, true);
	shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	shader->setUniform("u_color", color);
	shader->setUniform("u_percentage", percentage);
	shader->setUniform("u_decrease", 0.0f);

	square.render(GL_TRIANGLES);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}


void GameStage::renderPic(Vector2 position, Vector2 size, Texture* diffuse) {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	picshader->enable();

	Mesh quad;
	quad.createQuad(position.x, position.y, size.x, size.y, false);

	Stage* stage = StageManager::instance->currStage;

	picshader->setUniform("u_model", Matrix44());
	picshader->setUniform("u_color", Vector4(1.0f));
	picshader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);

	if (diffuse) {
		picshader->setTexture("u_texture", diffuse, 0);
	}

	quad.render(GL_TRIANGLES);

	picshader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void GameStage::generateShadowMaps(Camera* camera)
{
	int shadowMapSize = 1024;

	for (int i = 0; i < lights.size(); ++i)
	{
		Light* light = lights[i];

		if (!light->cast_shadows || light->lightType == eLightType::POINTLIGHT)
			continue;

		if (light->shadowMapFBO == nullptr || light->shadowMapFBO->width != shadowMapSize)
		{
			if (light->shadowMapFBO)
				delete light->shadowMapFBO;
			light->shadowMapFBO = new FBO();
			light->shadowMapFBO->setDepthOnly(shadowMapSize, shadowMapSize);
		}

		Camera light_camera;
		Vector3 up = Vector3(0, 1, 0);

		Vector3 pos = camera->eye;
		light_camera.lookAt(pos, pos - light->getFront(), up);

		light->shadowMapFBO->bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		if (light->lightType == eLightType::DIRECTIONAL)
		{
			//light = mainLight;
			//pos = camera->eye;
			light_camera.lookAt(pos, pos - light->getFront(), up);

			float halfArea = light->area / 2.0f;
			light_camera.setOrthographic(-halfArea, halfArea, -halfArea, halfArea, light->near_distance, light->max_distance);

			//compute texel size in world units, where frustum size is the distance from left to right in the camera
			float grid = light->area / (float)shadowMapSize;

			//snap camera X,Y to that size in camera space assuming the frustum is square, otherwise compute gridx and gridy
			light_camera.view_matrix.M[3][0] = round(light_camera.view_matrix.M[3][0] / grid) * grid;

			light_camera.view_matrix.M[3][1] = round(light_camera.view_matrix.M[3][1] / grid) * grid;

			//update viewproj matrix (be sure no one changes it)
			light_camera.viewprojection_matrix = light_camera.view_matrix * light_camera.projection_matrix;

		}
		else if (light->lightType == eLightType::SPOT)
		{
			light_camera.setPerspective(light->cone_info.y * 2.0f, 1.0f, light->near_distance, light->max_distance);
		}

		light->shadowMap_viewProjection = light_camera.viewprojection_matrix;
		light_camera.enable();

		root_opaque->render(&light_camera);

		light->shadowMapFBO->unbind();
	}
}

void GameStage::shadowToShader(Light* light, int& shadowMapPos, Shader* shader)
{
	shader->setUniform("u_light_cast_shadows", 1);
	shader->setUniform("u_shadowmap", light->shadowMapFBO->depth_texture, 8);
	shader->setUniform("u_shadowmap_viewprojection", light->shadowMap_viewProjection);
	shader->setUniform("u_shadow_bias", light->shadow_bias);
}

void GameStage::lightToShader(Light* light, Shader* shader)
{
	Vector2 cone_info = Vector2(cosf(light->cone_info.x * PI / 180.0f), cosf(light->cone_info.y * PI / 180.0f));

	shader->setUniform("u_light_type", (int)light->lightType);
	shader->setUniform("u_light_position", light->model.getTranslation());
	shader->setUniform("u_light_front", light->getFront());
	shader->setUniform("u_light_color", light->color * light->intensity);
	shader->setUniform("u_light_max_distance", light->max_distance);
	shader->setUniform("u_light_cone_info", cone_info);

	int shadowMapPos = 8;
	if (light->cast_shadows) GameStage::shadowToShader(light, shadowMapPos, shader);
	else shader->setUniform("u_light_cast_shadows", 0);
}

//what to do when the image has to be draw
void GameStage::render(void)
{
	float width = Game::instance->window_width, height = Game::instance->window_height;
	generateShadowMaps(camera);
	if (!renderFBO) {
		renderFBO = new RenderToTexture();
		renderFBO->create(width, height);
	}
	renderFBO->enable();
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	renderSkybox(cubemap);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	drawGrid();

	root_opaque->renderWithLights(camera);

	std::sort(root_transparent->children.begin(), root_transparent->children.end(), compareFunction);

	enemy->renderWithLights(camera);
	player->renderWithLights(camera);

	root_transparent->renderWithLights(camera);

	glDisable(GL_DEPTH_TEST);




	renderFBO->disable();

	Shader* shader = Shader::Get("data/shaders/postfx.vs", "data/shaders/postfx.fs");
	shader->enable();
	shader->setUniform("iResolution", Vector2(renderFBO->width, renderFBO->height));

	renderFBO->toViewport(shader);
	//mainLight->shadowMapFBO->depth_texture->toViewport();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	drawText(2, 400, std::to_string(player->targetable), Vector3(1, 1, 1), 5);

	//amogus.render(camera2D);
	renderHUD();
}

bool GameStage::compareFunction(const Entity* e1, const Entity* e2) {
	Stage* stage = StageManager::instance->currStage;
	EntityMesh* em1 = (EntityMesh*)e1;
	EntityMesh* em2 = (EntityMesh*)e2;
	Vector3 center_e1 = e1->model * em1->mesh->box.center;
	Vector3 center_e2 = e2->model * em2->mesh->box.center;
	return stage->camera->eye.distance(center_e1) > stage->camera->eye.distance(center_e2);
}

void GameStage::update(double seconds_elapsed)
{
	if (anxiety_dt != 0) {
		anxiety += anxiety_dt * seconds_elapsed * 10;
		anxiety_dt -= anxiety_dt * seconds_elapsed * 10;
	}
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	// e2->model.rotate(angle * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

	// Example
	angle += (float)seconds_elapsed * 10.0f;
	// Mouse input to rotate the cam
	Vector3 cam_ground = cam_position * Vector3(1, 0, 1);
	float box_dist = player->getPositionGround().distance(cam_ground);
	if (box_dist > 1) {
		cam_position += (box_dist - 1) * (player->getPositionGround() - cam_ground) * seconds_elapsed;
	}
	// Async input to move the camera around
	//if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (!mouse_locked) {
		if (Input::isMousePressed(SDL_BUTTON_LEFT)) { //is left button pressed?
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
			camera->move(Vector3(Input::mouse_delta.x * 0.5f, Input::mouse_delta.y * 0.005f, 0.0));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}
	else {
		Vector3 player_pos = cam_position * Vector3(1,0,1);
		Vector3 enemy_pos = enemy->getPosition();
		Vector3 director = player_pos - enemy_pos;

		Vector3 cam_pos = player_pos + director.normalize() * (3 * zoom) + Vector3(0, 5 + 1 * zoom, 0);

		//std::vector<sCollisionData> cols;
		//ray_collided(root, cols, cam_pos, Vector3::UP, 200, WALL);
		//for (sCollisionData& c : cols) {
		//	cam_pos.y += (c.colPoint.y - cam_pos.y);
		//}

		camera->lookAt(cam_pos, enemy_pos, camera->up);
	}
	// camera->lookAt(player->model);
	/*float zdiff = player->model.getTranslation().z - e2->model.getTranslation().z;
	float sign; zdiff >= 0 ? sign = 1 : sign = -1;
	float reverse_dist = 1 / sqrt(clamp(player->distance(e2) / 1000, 0.1, 2.5));
	camera->lookAt((2*reverse_dist)*(player->model.getTranslation() - e2->model.getTranslation()) + Vector3(0,500 - player->model.getTranslation().y * 2 * reverse_dist, 0), e2->model.getTranslation() + Vector3(0, 200, 0), camera->up);*/


	if (trees_shoot + interval < Game::instance->time) {
		for (Entity* e : root_transparent->children) {
			EntityMesh* ee = dynamic_cast <EntityMesh*> (e);
			if (ee == nullptr) continue;
			if (ee->type == WALL) {
				float r = random();
				if (r>0.8){
					Matrix44 _m = Matrix44();
					Vector3 center_world = ee->model * ee->mesh->box.center;
					_m.translate(center_world);
					for (int i = 0; i < 12; i++) {
						_m.rotate(PI / 6, Vector3::UP);
						Matrix44 __m = _m;
						__m.translate(Vector3(1, -3.2, 0));
						Patterns::circle3(__m, enemy->bullets_ball, 1, 0, 1);
					}
				}
			}
		}
		trees_shoot = Game::instance->time;
		interval = random(6, 6);
	}


	if (columns_shoot + columns_interval < Game::instance->time) {
		for (Entity* e : root_transparent->children) {
			EntityMesh* ee = dynamic_cast <EntityMesh*> (e);
			if (ee == nullptr) continue;
			if (ee->type == COLUMN) {
				Matrix44 _m = Matrix44();
				Vector3 center_world = ee->model * ee->mesh->box.center;
				_m.translate(center_world);
				for (int i = 0; i < 6; i++) {
					_m.rotate(PI / 3, Vector3::UP);
					Matrix44 __m = _m;
					__m.translate(Vector3(2, -1, 0));
					Patterns::circle5(__m, enemy->bullets_giantball, Vector2(0, 2), Vector2(-2, 2), 1, 0.7, 0.1, 0.5, -0.25, 0.0);
					//Patterns::circle3(__m, enemy->bullets_ball, 1, 0, 1);
				}
			}
		}
		columns_shoot = Game::instance->time;
		columns_interval = 5;
	}
	enemy->update(seconds_elapsed);
	player->update(seconds_elapsed);
}

//Keyboard event handler (sync input)
void GameStage::onKeyDown(SDL_KeyboardEvent event)
{
	player->onKeyDown(event);
	if (event.keysym.sym == SDLK_p) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}
}

void GameStage::onKeyUp(SDL_KeyboardEvent event)
{
	player->onKeyUp(event);
}

void GameStage::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}
}

void GameStage::onMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void GameStage::onMouseWheel(SDL_MouseWheelEvent event)
{
	if (!mouse_locked) mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
	else zoom += event.y > 0 ? 0.05f : -0.05f;

}

void GameStage::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void GameStage::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void GameStage::resize()
{
	Stage::resize();
	float width = Game::instance->window_width, height = Game::instance->window_height;

	if (!renderFBO) renderFBO = new RenderToTexture();
	renderFBO->create(width, height);
}