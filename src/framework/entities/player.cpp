#include "player.h"


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

void Player::shoot(bullet_type bullet_type = auto_aim) {
	if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet > shoot_cooldown[bullet_type]) {
		timer_bullet = Game::instance->time;
		mana -= shoot_cost[bullet_type];
		free_bullets -= amount[bullet_type];

		patterns[bullet_type](Vector3(0,0,0), forward, model, bullets, amount[bullet_type]);
		std::cout << mana << " " << bullet_idx_first << " " << free_bullets << " " << bullet_type << std::endl;
	}
}

Vector3 Player::getPosition() {
	return model.getTranslation();
}

Vector3 Player::getPositionGround() {
	return Vector3(model.getTranslation().x, 0, model.getTranslation().z);
}

void Player::render(Camera* camera) {
	EntityMesh::render(camera);
	// Render Bullets
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->render(camera);
	}
	// Entity::render(camera);
};


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
		shoot(bt);
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
		if (Game::instance->time - b->timer_spawn > 5) {
			bullets.erase((bullets.begin() + i));
			delete b;
			bullet_idx_last++;
			free_bullets++;
		}
		else b->update(delta_time);
	}
	Entity::update(delta_time);

	mana += 10 * delta_time;

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

void Player::onKeyDown(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_1)
	{
		bt = auto_aim;
	} 
	if (event.keysym.sym == SDLK_2) {
		bt = circle;
	}
	if (event.keysym.sym == SDLK_3) {
		bt = shotgun;
	}
	if (event.keysym.sym == SDLK_4) {
		bt = sniper;
	}
}