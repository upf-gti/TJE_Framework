#include "GameStage.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "graphics/material.h"
#include "framework/entities/enemy.h"
#include "framework/entities/player.h"
#include "framework/entities/entityUI.h"
#include "StageManager.h"

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
Shader* shader = NULL;
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

EntityUI amogus;

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

static bool parseScene(const char* filename, Entity* root)
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

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = WALL;
			// Create a different type of entity
			// new_entity = new ...
		}
		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
			new_entity->type = FLOOR;
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
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
			if (!new_entity->material.shader) new_entity->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturepixel.fs");
		}

		std::cout << " " << &new_entity->material.shader << std::endl;

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

bool GameStage::ray_collided(std::vector<sCollisionData>& ray_collisions, Vector3 position, Vector3 direction, float dist, bool in_object_space, COL_TYPE collision_type) {
	for (int i = 0; i < root->children.size(); ++i) {
		EntityMesh* ee = (EntityMesh*)root->children[i];
		if ((ee->type & collision_type) == 0) continue;
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


bool GameStage::sphere_collided(std::vector<sCollisionData>& collisions, Vector3 position, float radius, COL_TYPE collision_type) {
	Stage* stage = StageManager::instance->currStage;
	for (int i = 0; i < stage->root->children.size(); ++i) {

		EntityMesh* ee = (EntityMesh*) stage->root->children[i];
		if (!(ee->type & collision_type)) continue;

		/*if (collision_type == NONE) {
			if (!(ee->type & collision_type)) {
				std::cout << "Skipping entity due to type mismatch." << std::endl;
				continue;
			}
			else {
				std::cout << "Type " + ee->type << std::endl;
			}
		}*/


		sCollisionData data;

		if (ee->isInstanced) {
			for (Matrix44& instanced_model : ee->models) {
				if (ee->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal)) {
					collisions.push_back(data);
				}
			}
		}
		else {
			if (ee->mesh->testSphereCollision(ee->model, position, radius, data.colPoint, data.colNormal)) {
				collisions.push_back(data);
			}
		}
	}

	return (!collisions.empty());
}

GameStage::GameStage()
{
	mouse_locked = false;

    nextStage = "EndStage";

	player = new Player();
	Material* mat = new Material();
	mat->color = Vector4(1, 1, 1, 1);

	mat->shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	mat->diffuse = Texture::Get("data/textures/asian.tga");
	player->mesh = Mesh::Get("data/meshes/player.MESH");

	shader = Shader::Get("data/shaders/hud.vs", "data/shaders/texture.fs");
	sus = Texture::Get("data/gus.tga");
	Material amogus_mat = Material();
	amogus_mat.color = Vector4(1.0f);
	amogus_mat.diffuse = sus;
	amogus_mat.shader = shader;
	amogus = EntityUI();
	amogus.material = amogus_mat;
	Mesh* _quad = new Mesh();
	_quad->createQuad(300, 400, 200, 100, false);
	amogus.mesh = _quad;


	player->material = *mat;
	player->isAnimated = true;
	e2 = new Player();
	e2->model.setTranslation(Vector3(10, 0, 5));
	player->model.setTranslation(Vector3(1, 0, 1));
	player->box_cam = Vector3(0, 0, 10);
	// AAA
	enemy = new Enemy(player->mesh, *mat, "Francisco", true, 1);
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

	//// Three vertices of the 1st triangle
	//quad.vertices.push_back(Vector3(-1, 1, 0));
	//quad.uvs.push_back(Vector2(0, 1));
	//quad.vertices.push_back(Vector3(-1, -1, 0));
	//quad.uvs.push_back(Vector2(0, 0));
	//quad.vertices.push_back(Vector3(1, -1, 0));
	//quad.uvs.push_back(Vector2(1, 0));

	//// Three vertices of the 2nd triangle
	//quad.vertices.push_back(Vector3(-1, 1, 0));
	//quad.uvs.push_back(Vector2(0, 1));
	//quad.vertices.push_back(Vector3(1, -1, 0));
	//quad.uvs.push_back(Vector2(1, 0));
	//quad.vertices.push_back(Vector3(1, 1, 0));
	//quad.uvs.push_back(Vector2(1, 1));

	// Hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	root = new Entity();
	parseScene("data/myscene.scene", root);

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
	root->addChild(player);
	root->addChild(enemy);
}


// Assuming you have a function to loadTGA that reads the TGA data


//what to do when the image has to be draw
void GameStage::render(void)
{
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	renderSkybox(cubemap);

	drawGrid();


	root->render(camera);
	//enemy->render(camera);
	// Draw the floor grid


	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	drawText(2, 50, std::to_string((player->currHP / player->maxHP) * 100) + '%', Vector3(1, 1, 1), 2);
	drawText(2, 400, std::to_string(floor(player->mana)), Vector3(1, 1, 1), 5);
	drawText(Game::instance->window_width / 2.0f, Game::instance->window_height - 100, std::to_string(enemy->currHP), Vector3(1, 1, 1), 5);

	//amogus.render(camera2D);

	//Camera camera2D;
	//camera2D.enable();
	//camera2D.view_matrix = Matrix44(); // Set View to identity
	//camera2D.setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//shader->enable();
	//shader->setUniform("u_model", Matrix44());
	//shader->setUniform("u_color", Vector4(1.0f));
	//shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	//shader->setTexture("u_texture", sus, 0);

	//quad->render(GL_TRIANGLES);

	//shader->disable();

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);


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
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
	player->update(seconds_elapsed);
	// e2->model.rotate(angle * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));


	std::sort(root->children.begin(), root->children.end(), compareFunction);

	// Example
	angle += (float)seconds_elapsed * 10.0f;
	// Mouse input to rotate the cam


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
		Vector3 player_pos = player->box_cam;
		Vector3 enemy_pos = enemy->getPosition();
		Vector3 director = player_pos - enemy_pos;
		camera->lookAt(player_pos + director.normalize() * (2 * zoom) + Vector3(0, 1 + 1 * zoom, 0), enemy_pos, camera->up);
	}
	// camera->lookAt(player->model);
	/*float zdiff = player->model.getTranslation().z - e2->model.getTranslation().z;
	float sign; zdiff >= 0 ? sign = 1 : sign = -1;
	float reverse_dist = 1 / sqrt(clamp(player->distance(e2) / 1000, 0.1, 2.5));
	camera->lookAt((2*reverse_dist)*(player->model.getTranslation() - e2->model.getTranslation()) + Vector3(0,500 - player->model.getTranslation().y * 2 * reverse_dist, 0), e2->model.getTranslation() + Vector3(0, 200, 0), camera->up);*/
	enemy->update(seconds_elapsed);

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