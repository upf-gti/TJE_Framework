#include "player.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

// cubemap

void Player::dash(float delta_time, float dash_duration = 1, float invul_duration = 0.3) {
	if (!dashing) {
		m_spd = 4 * DEFAULT_SPD;
		timer_dash = Game::instance->time;
		dashing = true;
		can_be_hit = false;
	}
	else if (m_spd > DEFAULT_SPD) {
		m_spd -= 4 * DEFAULT_SPD * delta_time / dash_duration; 
	}
	else {
		m_spd = DEFAULT_SPD;
		dashing = false;
	}
	if (!can_be_hit && Game::instance->time - timer_dash > invul_duration) {
		can_be_hit = true;
	}
}

void Player::jump(float delta_time) {
	if (grounded) {
		v_spd = JUMP_SPD;
		timer_jump = Game::instance->time;
		grounded = false;
		jumping = true;
	}
}

void Player::shoot(uint8 bullet_type = 0) {
	if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet > shoot_cooldown[bullet_type]) {
		timer_bullet = Game::instance->time;
		mana -= shoot_cost[bullet_type];
		free_bullets--;
		switch (bullet_type) {
		case 0:
			Material mat = Material();
			Texture* texture = Texture::Get("data/textures/texture.tga");
			Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturepixel.fs");
			
			Mesh* mesh = Mesh::Get("data/meshes/box.ASE");

			mat.diffuse = texture;
			mat.shader = shader;

			Bullet * b = new Bullet(mesh, mat);
			b->direction = forward;
			b->objective = Vector3(0, 700, 0); b->has_objective = true;
			b->model = model;
			bullets.push_back(b);
			bullet_idx_first = (bullet_idx_first + 1) % MAX_BULLETS;
			break;
		}
		std::cout << mana << " " << bullet_idx_first << " " << free_bullets << " " << std::endl;
	}
}
Vector3 Player::getPosition() {
	return model.getTranslation();
}

Vector3 Player::getPositionGround() {
	return Vector3(model.getTranslation().x, 0, model.getTranslation().z);
}

void Player::render(Camera* camera) {
	//if (!mesh) {
	//	std::cout << "no mesh";
	//	return;
	//}

	//// Set flags
	//glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	////// Get the last camera that was activated 
	////Camera* camera = Camera::current;

	//Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");

	//// Enable shader and pass uniforms 
	//material.shader->enable();

	//material.shader->setUniform("u_model", model);
	//material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	//if (material.diffuse) {
	//	material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	//}

	//if (!isInstanced) {
	//	material.shader->setUniform("u_model", model);
	//}
	//if (isInstanced)
	//	mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	//else
	//	// Render the mesh using the shader
	//	mesh->render(GL_TRIANGLES);

	//// Disable shader after finishing rendering
	//material.shader->disable();


	// Render hijos

	//for (size_t i = 0; i < children.size(); i++)
	//{
	//	children[i]->render(camera);
	//}
	// Or just update the father one
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->render(camera);
	}
	Entity::render(camera);
};

void Player::addInstance(const Matrix44& model)
{
}

void Player::move(Vector3 vec) {
	model.translate(vec);

}

void Player::update(float delta_time) {
	if (/*Input::isMousePressed(SDL_BUTTON_LEFT) || */Game::instance->mouse_locked) //is left button pressed?
	{
		model.rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_LSHIFT) || dashing){
		dash(delta_time);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		jump(delta_time);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Q)) {
		shoot();
	}
	if (!grounded && !jumping) {
		v_spd -= GRAVITY * delta_time;
	}
	if ((Input::isKeyPressed(SDL_SCANCODE_W) || 
		Input::isKeyPressed(SDL_SCANCODE_S) || 
		Input::isKeyPressed(SDL_SCANCODE_A) || 
		Input::isKeyPressed(SDL_SCANCODE_D)) && !dashing && Game::instance->mouse_locked) m_spd = DEFAULT_SPD;
	if (!dashing && m_spd > 0) {
		m_spd -= DEFAULT_SPD * delta_time / stop_duration;
		if (m_spd < 0) m_spd = 0;
	}

	float speed = m_spd; //the speed is defined by the seconds_elapsed so it goes constant

	// std::cout << v_spd << " " << grounded << " " << jumping << std::endl;

	if (Input::isKeyPressed(SDL_SCANCODE_W)) direction = forward;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) direction = -forward;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) direction = right;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) direction = -right;

	move(direction * speed * delta_time);
	move(Vector3(0, 1, 0) * v_spd * delta_time);
	if (model.getTranslation().y < 0) {
		move(Vector3(0, -model.getTranslation().y, 0));
		grounded = true;
	}
	for (int i = 0; i < bullets.size(); i++) {
		Bullet* b = bullets[i];
		if (Game::instance->time - b->timer_spawn > 5 || b->model.getTranslation().distance(b->objective) < 50) {
			bullets.erase((bullets.begin() + i));
			delete b;
			bullet_idx_last++;
			free_bullets++;
		}
		else b->update(delta_time);
	}
	Entity::update(delta_time);

	//std::cout << model.getTranslation().x << " "
	//	<< model.getTranslation().y << " "
	//	<< model.getTranslation().z << std::endl;
}



void Player::onMouseWheel(SDL_MouseWheelEvent event)
{
	m_spd *= event.y > 0 ? 1.1f : 0.9f;
}

void Player::onMouseButtonDown(SDL_MouseButtonEvent event)
{

}

void Player::onKeyUp(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_SPACE)
	{
		std::cout << "hehe" << std::endl;
		jumping = false;
	}
}