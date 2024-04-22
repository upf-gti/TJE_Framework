#include "player.h"
#include "framework/camera.h"
#include "framework/input.h"

#include <algorithm>


void Player::render(Camera* camera) {
	if (!mesh) {
		std::cout << "no mesh";
		return;
	}

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	//// Get the last camera that was activated 
	//Camera* camera = Camera::current;

	Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");

	// Enable shader and pass uniforms 
	material.shader->enable();

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	
	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	}

	if (!isInstanced) {
		material.shader->setUniform("u_model", model);
	}
	if (isInstanced)
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	else
		// Render the mesh using the shader
		mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();


	// Render hijos

	//for (size_t i = 0; i < children.size(); i++)
	//{
	//	children[i]->render(camera);
	//}
	// Or just update the father one
	Entity::render(camera);
};

void Player::addInstance(const Matrix44& model)
{
}

void Player::move(Vector3 vec) {
	model.translate(vec);
	
}

void Player::update(float delta_time) {
	// Async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_I) || Input::isKeyPressed(SDL_SCANCODE_UP)) move(Vector3(0.0f, 0.0f, 1.0f) * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_K) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) move(Vector3(0.0f, 0.0f, -1.0f) * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_J) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) move(Vector3(1.0f, 0.0f, 0.0f) * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_L) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) move(Vector3(-1.0f, 0.0f, 0.0f) * speed * delta_time);
	
	Entity::update(delta_time);
}