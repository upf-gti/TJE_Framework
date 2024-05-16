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
	else v_spd = JUMP_SPD * (1 - (2*(Game::instance->time - timer_jump)));
}

void Player::shoot(bullet_type bullet_type = auto_aim) {
	if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet[bullet_type] > shoot_cooldown[bullet_type]) {
		timer_bullet[bullet_type] = Game::instance->time;
		mana -= shoot_cost[bullet_type];
		free_bullets -= amount[bullet_type];

		patterns[bullet_type](Vector3(0,0,0), forward, model, bullets, amount[bullet_type]);
		std::cout << mana << " " << bullet_idx_first << " " << free_bullets << " " << bullet_type << std::endl;
	}
}

void Player::shootCharge(bullet_type bullet_type) {
	//std::cout << "charging (telo imaginas)" << std::endl;
	bool time = (Game::instance->time - timer_charge[bullet_type]) > charge_cooldown[bullet_type];

	if (time && !charging && Game::instance->time - timer_bullet[bullet_type] > shoot_cooldown[bullet_type]) {
		timer_charge[bullet_type] = Game::instance->time;
		charging = true;
	}
	else if (charging) {
		if (!time) {
			
		}
		else {
			charging = false;
			shoot(bullet_type);
		}
	}
}

Vector3 Player::getPosition() {
	return model.getTranslation();
}

Vector3 Player::getPositionGround() {
	return Vector3(model.getTranslation().x, 0, model.getTranslation().z);
}

void Player::render(Camera* camera) {

	// Render Bullets
	for (int i = bullets.size()-1; i >= 0; i--) {
		bullets[i]->render(camera);
	}

	bool time = (Game::instance->time - timer_charge[bt]) > charge_cooldown[bt];
	if (charging) {
		float size = (1 - (Game::instance->time - timer_charge[bt])) / charge_cooldown[bt];
		charge_model = model;
		charge_model.translate(Vector3(0, 1, 0));
		charge_model.scale(size, size, size);

		if (!charge_mat.shader) {
			charge_mat.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
		}

		charge_mat.shader->enable();

		charge_mat.shader->setUniform("u_color", charge_mat.color);
		charge_mat.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		charge_mat.shader->setTexture("u_texture", charge_mat.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
		charge_mat.shader->setUniform("u_model", charge_model);
		charge_mat.shader->setUniform("u_time", Game::instance->time);

		charge_mesh->render(GL_TRIANGLES);

		// Disable shader after finishing rendering
		charge_mat.shader->disable();
	}

	// Render sphere

	Matrix44 m = model;

	flat_shader->enable();

	float sphere_radius = HITBOX_RAD;
	m.translate(0.0f, player_height, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	flat_shader->setUniform("u_color", Vector4(touching_ground, 0.0f, colliding, 1.0f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", m);

	hitbox_mesh->render(GL_LINES);

	flat_shader->disable();

	Matrix44 squash = model;
	
	squash.setTranslation(Vector3(model.getTranslation().x, ground_y, model.getTranslation().z));
	squash.scale(1 / (1 + ground_below_y), 1 / (1 + ground_below_y), 1 / (1 + ground_below_y));

	glDepthFunc(GL_GREATER);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDepthMask(false);
	flat_shader->enable();


	flat_shader->setUniform("u_color", Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", squash);

	shadow_mesh->render(GL_TRIANGLES);

	flat_shader->disable();
	glDisable(GL_BLEND);
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDepthMask(true);
	// Entity::render(camera);
	EntityMesh::render(camera);
};




void Player::move(Vector3 vec) {
	//model.translate(vec);
	model.translateGlobal(vec);
}



void Player::update(float delta_time) {
	// std::cout << grounded << std::endl;

	float time = Game::instance->time;
	float box_dist = getPositionGround().distance(box_cam);
	if (box_dist > 1) {
		box_cam += (box_dist - 1) * (getPositionGround() - box_cam) * delta_time;
	}
	timer_bullet_general = Game::instance->time - timer_bullet[bt];
	if (Input::wasKeyPressed(SDL_SCANCODE_LSHIFT) || dashing) {
		dash(delta_time);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
		//std::cout << std::endl << "\ncharge:\n" << charge_cooldown[bt] << std::endl;
		if (charge_cooldown[bt]) shootCharge(bt);
		else shoot(bt);
	}
	else charging = false;
	if (autoshoot) {
		shoot();
	}

	if ((Input::isKeyPressed(SDL_SCANCODE_W) ||
		Input::isKeyPressed(SDL_SCANCODE_S) ||
		Input::isKeyPressed(SDL_SCANCODE_A) ||
		Input::isKeyPressed(SDL_SCANCODE_D)) && !dashing && Stage::instance->mouse_locked) m_spd = DEFAULT_SPD + DEFAULT_SPD * autoshoot / 2;

	if (!dashing && m_spd > 0) {
		m_spd -= DEFAULT_SPD * delta_time / stop_duration;
		if (m_spd < 0) m_spd = 0;
	}
	bool is_knowckback = timer_bullet_general < knockback_time[bt];
	float speed = m_spd - DEFAULT_SPD * knockback[bt] * (knockback_time[bt] - timer_bullet_general) * (is_knowckback); //the speed is defined by the seconds_elapsed so it goes constant

	if (/*Input::isMousePressed(SDL_BUTTON_LEFT) || */Stage::instance->mouse_locked) //is left button pressed?
	{
		model.rotate(Input::mouse_delta.x * (0.005f - (timer_bullet_general < knockback_time[bt]) * (0.0045f)), Vector3(0.0f, -1.0f, 0.0f));
	}
	direction = model.frontVector();

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

	if (mana < 200) {
		mana += (DEFAULT_COST + 3) * delta_time / (DEFAULT_FIRERATE);
	}
	else mana = 200;

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground;

	Vector3 player_center = model.getTranslation() + Vector3(0, player_height, 0);

	colliding = Stage::instance->sphere_collided(collisions, player_center, HITBOX_RAD);
	Stage::instance->ray_collided(ground, player_center, -Vector3::UP, 100);

	for (sCollisionData& g : collisions) {
		direction += g.colNormal * 10;
		direction.y = 0;
		direction.normalize();
	}

	ground_below_y = 10000;
	ground_y = 10000;

	touching_ground = false;

	for (sCollisionData& g : ground) {
		ground_below_y = player_center.y - g.colPoint.y;
		if (player_center.y - g.colPoint.y < player_height + 0.01) {
			touching_ground = true;
			v_spd = g.colNormal.y * ground_below_y * ground_below_y;
		}
		ground_y = g.colPoint.y;
		ground_normal = g.colNormal;
	}
	grounded = touching_ground;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && (grounded || ((time - timer_jump) < .3))) {
		jump(delta_time);
	}
	else {
		jumping = false;
	}
	if (!grounded && !jumping) {
		v_spd -= GRAVITY * delta_time;
	}
	move(direction * speed * delta_time);
	if (!grounded)
		move(Vector3::UP * v_spd * delta_time);
	else
		move(Vector3::UP * (ground_y - getPosition().y) * delta_time * 20);
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
	if (event.keysym.sym == SDLK_q) {
		canshoot = true;
	}
}

void Player::onKeyDown(SDL_KeyboardEvent event)
{
	if (timer_bullet_general > knockback_time[bt]) {
		if (event.keysym.sym == SDLK_1) {
			bt = circle;
		}
		if (event.keysym.sym == SDLK_2) {
			bt = shotgun;
		}
		if (event.keysym.sym == SDLK_3) {
			bt = sniper;
		}
	}
	if (event.keysym.sym == SDLK_SPACE && grounded) {
		timer_jump = Game::instance->time;
	}
	if (event.keysym.sym == SDLK_e) {
		autoshoot = !autoshoot;
	}
}