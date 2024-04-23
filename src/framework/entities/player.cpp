#include "player.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

float m_spd = 100.0f;

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
	float speed = m_spd; //the speed is defined by the seconds_elapsed so it goes constant
	if (Input::isMousePressed(SDL_BUTTON_LEFT)) //is left button pressed?
	{
		model.rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
	}
	// Async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_I) || Input::isKeyPressed(SDL_SCANCODE_UP)) move(forward * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_K) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) move(-forward * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_J) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) move(right * speed * delta_time);
	if (Input::isKeyPressed(SDL_SCANCODE_L) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) move(-right * speed * delta_time);
	
	Entity::update(delta_time);
}

void Player::onMouseWheel(SDL_MouseWheelEvent event)
{
	m_spd *= event.y > 0 ? 1.1f : 0.9f;
}