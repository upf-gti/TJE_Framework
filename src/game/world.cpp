#include "world.h"

#include "framework/camera.h"
#include "framework/input.h"
#include "framework/entities/entity_mesh.h"
#include "framework/entities/entity_collider.h"

#include "graphics/texture.h"
#include "graphics/shader.h"

#include "game/game.h"
#include "stages/stage.h"

#include "entities/entity_player.h"
#include "framework/entities/entity_enemy.h"

#include <algorithm>
#include <fstream>

World* World::instance = nullptr;

World::World()
{
	int window_width = Game::instance->window_width;
	int window_height = Game::instance->window_height;

	// Create our 3D camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 20.f, -20.f), Vector3(0.f, 0.f, 0.f), Vector3::UP); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.01f, 1000.f); //set the projection, we want to be perspective

	//Creating the player
	Material player_material;
	player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	player_material.diffuse = new Texture();
	player_material.diffuse->load("data/textures/player.png");
	player = new EntityPlayer(Mesh::Get("data/meshes/player/player.obj"), player_material, "player");

	enemy = new EntityEnemy(Mesh::Get("data/meshes/player/player.obj"), "zombie_2");
	enemy->setLayer(eCollisionFilter::ENEMY);


	//Skybox
	Material landscape_cubemap;
	landscape_cubemap.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");
	landscape_cubemap.diffuse = new Texture();
	landscape_cubemap.diffuse->loadCubemap("landscape", {
		"data/textures/skybox/nx.png",
		"data/textures/skybox/ny.png" ,
		"data/textures/skybox/nz.png" ,
		"data/textures/skybox/px.png" ,
		"data/textures/skybox/py.png" ,
		"data/textures/skybox/pz.png" });

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.ASE"), landscape_cubemap, "landscape");

	parseScene("data/myscene.scene", &root);
}

void World::render() {
	//Set the camera as default
	camera->enable();

	// Draw the floor grid
	drawGrid();

	//Render skybox
	glDisable(GL_DEPTH_TEST);
	skybox->render(camera);
	glEnable(GL_DEPTH_TEST);

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//Render entity player
	player->render(camera);

	//Render all scene tree
	root.render(camera);
}

void World::update(float seconds_elapsed)
{
	if (free_camera)
	{
		float speed = seconds_elapsed * camera_speed;

		// Mouse input to rotate the cam
		if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
		}

		// Async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}
	else
	{
		//Update our scene
		root.update(seconds_elapsed);

		//and the player
		player->update(seconds_elapsed);

		//Get mouse deltas
		camera_yaw -= Input::mouse_delta.x * seconds_elapsed * mouse_speed;
		camera_pitch -= Input::mouse_delta.y * seconds_elapsed * mouse_speed;

		//Restrict pitch angle
		camera_pitch = clamp(camera_pitch, -M_PI * 0.4f, M_PI * 0.4f);

		Matrix44 mYaw;
		mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

		Matrix44 mPitch;
		mPitch.setRotation(camera_pitch, Vector3(-1, 0, 0));

		Vector3 front = (mPitch * mYaw).frontVector().normalize();
		Vector3 eye;
		Vector3 center;

		eye = player->model.getTranslation() + Vector3(0.f, 0.2f, 0.0f) + front * 0.1f; //0.2f enlloc de 0.1f per donar-li una mica més d'alçaddda al eye del player
		center = eye + front;

		//Ray-mesh intersection to move the camera up to the collision point

		Vector3 dir = eye - center;

		sCollisionData data = World::get_instance()->raycast(center, dir, eCollisionFilter::ALL, dir.length());

		if (data.collided) {
			eye = data.colPoint;
		}

		camera->lookAt(eye, center, Vector3(0, 1, 0));

	}


	//Delete pending entities
	for (auto e : entities_to_destroy) {
		root.removeChild(e);
		delete e;
	}

	entities_to_destroy.clear();
}

bool World::parseScene(const char* filename, Entity* root)
{
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::map < std::string, sRenderData> meshes_to_load;

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

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
		EntityMesh* new_entity = nullptr;

		size_t tag = data.first.find("@enemy");

		if (tag != std::string::npos) {
			/*Mesh* mesh = Mesh::Get("data/meshes/player/player.obj");
			// Create a different type of entity
			new_entity = new EntityEnemy(mesh, "zombie_1");
			assert(new_entity);
			new_entity->model.setTranslation(render_data.models[0].getTranslation());*/
		}
		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());

			//mesh->isTextured(); //Crear el metode istextured i mirar si el material té Kd_texture

			//mat.shader = ...  //Si té textura, posar el material de textura i sinó el del color per vertex

			new_entity = new EntityMesh(mesh, mat);
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void World::addEntity(Entity* entity)
{
	root.addChild(entity);
}

void World::removeEntity(Entity* entity)
{
	entities_to_destroy.push_back(entity);
}

void World::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter)
{
	for (auto e : root.children)
	{
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec == nullptr) {
			continue;
		}
		ec->getCollisions(target_position, collisions, ground_collisions, eCollisionFilter::ALL);
	}
}

sCollisionData World::raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist) {
	sCollisionData data;

	for (auto e : root.children) {
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec == nullptr || !(ec->getLayer() & layer)) {
			continue;
		}


		Vector3 col_point;
		Vector3 col_normal;

		if (!ec->mesh->testRayCollision(ec->model, origin, direction, col_point, col_normal, max_ray_dist)); {
			continue;
		}

		data.collided = true;

		//There are collisions? Update if nearest

		float non_distance = (col_point - origin).length();
		if (non_distance < data.distance) {
			data.colPoint = col_point;
			data.colNormal = col_normal;
			data.distance = non_distance;
			data.collider = ec;
		}
	}
}
