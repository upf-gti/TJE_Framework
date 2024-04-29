#include "player.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>



void Player::dash(float delta_time, float dash_duration = 1, float invul_duration = 0.3) {
	if (!is_dashing) {
		m_spd = 4 * DEFAULT_SPD;
		timer_dash = Game::instance->time;
		is_dashing = true;
		can_be_hit = false;
	}
	else if (m_spd > DEFAULT_SPD) {
		m_spd -= 4 * DEFAULT_SPD * delta_time / dash_duration; 
	}
	else {
		m_spd = DEFAULT_SPD;
		is_dashing = false;
	}
	if (!can_be_hit && Game::instance->time - timer_dash > invul_duration) {
		can_be_hit = true;
	}
}

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
	if (Input::isMousePressed(SDL_BUTTON_LEFT)) //is left button pressed?
	{
		model.rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_LSHIFT) || is_dashing){
		dash(delta_time);
	}
	if ((Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_D)) && !is_dashing) m_spd = DEFAULT_SPD;
	if (!is_dashing && m_spd > 0) {
		m_spd -= DEFAULT_SPD * delta_time / stop_duration;
		if (m_spd < 0) m_spd = 0;
	}

	float speed = m_spd; //the speed is defined by the seconds_elapsed so it goes constant
	// Async input to move the camera around

	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) direction = forward;
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) direction = -forward;
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) direction = right;
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) direction = -right;

	move(direction * speed * delta_time);

	Entity::update(delta_time);
}

void Player::onMouseWheel(SDL_MouseWheelEvent event)
{
	m_spd *= event.y > 0 ? 1.1f : 0.9f;
}