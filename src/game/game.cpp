#include "game.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "graphics/material.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/player.h"


#include <fstream>
#include <cmath>
#include <unordered_map>

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
float mouse_speed = 100.0f;

float camera_angle_x = 0;
float camera_angle_y = 100;

Game* Game::instance = NULL;
Player* player = NULL;
Player* e2 = NULL;
Matrix44 camera_support;

// Cosas nuevas que he añadido
Entity* root;


bool parseScene(const char* filename, Entity* root)
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
		EntityMesh* new_entity = nullptr;

		size_t tag = data.first.find("@tag");

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get("...");
			// Create a different type of entity
			// new_entity = new ...
		}
		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
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

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	player = new Player();
	e2 = new Player();
	e2->model.setTranslation(Vector3(1000, 0, 500));
	player->model.setTranslation(Vector3(0, 0, 10));

	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	// Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	// Load one texture using the Texture Manager
	texture = Texture::Get("data/textures/texture.tga");

	// Example of loading Mesh from Mesh Manager
	mesh = Mesh::Get("data/meshes/box.ASE");

	// Example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturepixel.fs");

	// Mesh* s = Mesh::Get();
	// 


	// Hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	root = new Entity();
	parseScene("data/myscene.scene", root);
}

//what to do when the image has to be draw
void Game::render(void)
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

	// Create model matrix for cube
	Matrix44 m = player->getGlobalMatrix();

	// Create model matrix for cube
	Matrix44 m2 = e2->getGlobalMatrix();

	if (shader)
	{
		// Enable shader
		shader->enable();

		// Upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", m2);
		shader->setUniform("u_time", time);

		// Do the draw call
		mesh->render(GL_TRIANGLES);

		// Disable shader
		shader->disable();
	}

	if (shader)
	{
		// Enable shader
		shader->enable();

		// Upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", m);
		shader->setUniform("u_time", time);

		// Do the draw call
		mesh->render(GL_TRIANGLES);

		// Disable shader
		shader->disable();
	}

	root->render(camera);
	player->render(camera);

	// Draw the floor grid
	drawGrid();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);


	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
	player->update(seconds_elapsed);
	// e2->model.rotate(angle * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

	// Example
	angle += (float)seconds_elapsed * 10.0f;
	// Mouse input to rotate the cam


	// Async input to move the camera around
	//if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (!mouse_locked) {
		if (Input::isMousePressed(SDL_BUTTON_LEFT)) { //is left button pressed?
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
			camera->move(Vector3(Input::mouse_delta.x * 0.5f, Input::mouse_delta.y * 0.005f,0.0));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}
	else {
		Vector3 player_pos = player->getPositionGround();
		Vector3 enemy_pos = e2->model.getTranslation();
		Vector3 director = player_pos - enemy_pos;
		camera->lookAt(player_pos + director.normalize() * (800) + Vector3(0, 500 , 0), enemy_pos, camera->up);
	}
	// camera->lookAt(player->model);
	/*float zdiff = player->model.getTranslation().z - e2->model.getTranslation().z;
	float sign; zdiff >= 0 ? sign = 1 : sign = -1;
	float reverse_dist = 1 / sqrt(clamp(player->distance(e2) / 1000, 0.1, 2.5));
	camera->lookAt((2*reverse_dist)*(player->model.getTranslation() - e2->model.getTranslation()) + Vector3(0,500 - player->model.getTranslation().y * 2 * reverse_dist, 0), e2->model.getTranslation() + Vector3(0, 200, 0), camera->up);*/


}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	}


	player->onKeyDown(event);
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	player->onKeyUp(event);
}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onResize(int width, int height)
{
	std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	camera->aspect = width / (float)height;
	window_width = width;
	window_height = height;
}
