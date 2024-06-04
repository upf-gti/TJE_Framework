#include "player.h"
#include "enemy.h"
#include "game/StageManager.h"
#include "framework/audio.h"

#include <algorithm>

// cubemap

void Player::sphere_bullet_collision(Vector3 position, float radius) {
	Stage* stage = StageManager::instance->currStage;
	for (Bullet* bullet : stage->enemy->bullets) {
		sCollisionData data;

		if (bullet->isInstanced) {
			for (Matrix44& instanced_model : bullet->models) {
				if (bullet->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal)) {
					colliding = true;
					bullet->to_delete = true;
					stage->anxiety += bullet->damage;
				}
			}
		}
		else {
			if (bullet->mesh->testSphereCollision(bullet->model, position, radius, data.colPoint, data.colNormal)) {
				colliding = true;
				bullet->to_delete = true;
				stage->anxiety += bullet->damage;
			}
		}
	}
	BulletNormal& bns = stage->enemy->bullets_normal;
	for (int i = 0; i < bns.models.size(); i++) {
		Matrix44& m = stage->enemy->bullets_normal.models[i];
		sCollisionData data;
		if (bns.mesh->testSphereCollision(m, position, radius, data.colPoint, data.colNormal)) {
			bns.models.erase((bns.models.begin() + i));
			bns.speeds.erase((bns.speeds.begin() + i));
			stage->anxiety += bns.damage;
		}
	}
}

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
	Matrix44 _m = model;
	_m.setFrontAndOrthonormalize(_m.frontVector() * Vector3(1, 0, 1));
	if (bullet_type == shotgun) {
		if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet[bullet_type] > shoot_cooldown[bullet_type]) {
			timer_bullet[bullet_type] = Game::instance->time;
			mana -= shoot_cost[bullet_type];
			free_bullets -= amount[bullet_type];
			std::cout << "Aqui llega \n";
			Patterns::shotgun2(_m, bullets_normal, 20);
			std::cout << mana << " " << bullet_idx_first << " " << free_bullets << " " << bullet_type << std::endl;
		}
		return;
	}
	else if (bullet_type == auto_aim) {
		if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet[bullet_type] > shoot_cooldown[bullet_type]) {
			timer_bullet[bullet_type] = Game::instance->time;
			mana -= shoot_cost[bullet_type];
			free_bullets -= amount[bullet_type];
			Patterns::autoAim2(_m, bullets_auto);
			std::cout << mana << " " << bullet_idx_first << " " << free_bullets << " " << bullet_type << std::endl;
		}
		return;
	}
	else if (free_bullets && mana > shoot_cost[bullet_type] && Game::instance->time - timer_bullet[bullet_type] > shoot_cooldown[bullet_type]) {
		timer_bullet[bullet_type] = Game::instance->time;
		mana -= shoot_cost[bullet_type];
		free_bullets -= amount[bullet_type];
		patterns[bullet_type](StageManager::instance->currStage->enemy->getPosition() + Vector3(0, player_height, 0), forward, _m, bullets, amount[bullet_type], bullet_shaders[bullet_type], bullet_textures[bullet_type], bullet_meshes[bullet_type]);
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


void Player::showHitbox(Camera* camera) {


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
}

void Player::chargingShot(Camera* camera) {
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

void Player::render(Camera* camera) {

	bool time = (Game::instance->time - timer_charge[bt]) > charge_cooldown[bt];
	// Render Bullets
	for (int i = bullets.size()-1; i >= 0; i--) 
		bullets[i]->render(camera);


	// Render the charging
	if (charging) 
		chargingShot(camera);

	// Render the Shadow
	Matrix44 squash = model;
	squash.setTranslation(Vector3(model.getTranslation().x, ground_y, model.getTranslation().z));
	squash.scale(1 / (1 + ground_below_y), 0.01, 1 / (1 + ground_below_y));

	//glDepthFunc(GL_GREATER);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glDepthMask(false);
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


	anim = animation_pool[current_animation];
	Skeleton blended_skeleton = anim->skeleton;
	if (Game::instance->time - timer_anim < 0.4) {
		blendSkeleton(&animation_pool[last_animation]->skeleton,
			&animation_pool[current_animation]->skeleton,
			(Game::instance->time - timer_anim) / 0.4, &blended_skeleton);
	}

	if (!material.shader) {
		material.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	}
	anim->assignTime(Game::instance->time);
	material.shader->enable();
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	}
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_time", Game::instance->time);

	mesh->renderAnimated(GL_TRIANGLES, &blended_skeleton);
	// std::cout << isAnimated << std::endl;

	// Disable shader after finishing rendering
	material.shader->disable();

	// Render hijos
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->render(camera);
	}
	bullets_normal.render(camera);
	bullets_auto.render(camera);

	showHitbox(camera);
};

void Player::move(const Vector3& vec) {
	//model.translate(vec);
	model.translateGlobal(vec);
}

void Player::update(float delta_time) {
	Stage* stage = StageManager::instance->currStage;
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

		Audio::Play("data/audio/whip.wav");
	}
	else charging = false;
	if (autoshoot) {
		shoot();
	}

	direction = model.frontVector();
	if ((Input::isKeyPressed(SDL_SCANCODE_W) ||
		Input::isKeyPressed(SDL_SCANCODE_L) ||
		Input::isKeyPressed(SDL_SCANCODE_A) ||
		Input::isKeyPressed(SDL_SCANCODE_D)) && !dashing /* && stage->mouse_locked*/) m_spd = DEFAULT_SPD;

	//direction = Vector3(0.0f);

	//if (Input::isKeyPressed(SDL_SCANCODE_W)) direction += getFront();
	//if (Input::isKeyPressed(SDL_SCANCODE_S)) direction -= getFront();
	//if (Input::isKeyPressed(SDL_SCANCODE_A)) direction += getRight();
	//if (Input::isKeyPressed(SDL_SCANCODE_D)) direction -= getRight();


	if (!dashing && m_spd > 0) {
		m_spd -= DEFAULT_SPD * delta_time / stop_duration;
		if (m_spd < 0) m_spd = 0;
	}
	direction *= m_spd;
	bool is_knowckback = timer_bullet_general < knockback_time[bt];
	float knockback_speed = DEFAULT_SPD * knockback[bt] * (knockback_time[bt] - timer_bullet_general) * (is_knowckback); //the speed is defined by the seconds_elapsed so it goes constant

	//std::cout << speed << std::endl;
	if (/*Input::isMousePressed(SDL_BUTTON_LEFT) || */stage->mouse_locked) //is left button pressed?
	{
		model.rotate(Input::mouse_delta.x * (0.005f - (timer_bullet_general < knockback_time[bt]) * (0.0045f)), Vector3(0.0f, -1.0f, 0.0f));
	}

	direction -= knockback_speed * model.frontVector();
	direction.normalize();
	float total_spd = abs(m_spd - knockback_speed);

	for (int i = 0; i < bullets.size(); i++) {
		Bullet* b = bullets[i];
		if (Game::instance->time - b->timer_spawn > 5 || b->to_delete) {
			bullets.erase((bullets.begin() + i));
			delete b;
			bullet_idx_last++;
			free_bullets++;
		}
		else b->update(delta_time);
	}
	bullets_auto.objective = stage->enemy->getHitboxPosition();
	bullets_normal.update(delta_time);
	bullets_auto.update(delta_time);
	Entity::update(delta_time);

	mana += (DEFAULT_COST + 3) * delta_time / (DEFAULT_FIRERATE);
	mana = clamp(mana, 0, 200);

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground;

	Vector3 player_center = getPosition() + Vector3(0, player_height, 0);
	if (!mesh) std::cout << "NOMESH ";
	colliding = stage->sphere_collided(stage->root, collisions, player_center, HITBOX_RAD);
	stage->ray_collided(stage->root, ground, player_center, -Vector3::UP, 1000000, FLOOR);

	for (sCollisionData& g : collisions) {
		direction += g.colNormal * 10000;
		direction.y = 0;
	}
	direction.normalize();
	ground_below_y = 10000;
	ground_y = -10000;

	touching_ground = false;

	for (sCollisionData& g : ground) {
		if (ground_y < g.colPoint.y) {
			ground_y = g.colPoint.y;
			ground_below_y = player_center.y - g.colPoint.y;
			ground_normal = g.colNormal;
		}
		if (player_center.y - g.colPoint.y < player_height + 0.01 && player_center.y - g.colPoint.y > player_height - 0.3) {
			touching_ground = true;
			v_spd = g.colNormal.y * ground_below_y * ground_below_y;
		}
	}
	grounded = touching_ground;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && (grounded || ((time - timer_jump) < .3)))
		jump(delta_time);
	else
		jumping = false;

	if (!grounded && !jumping) {
		v_spd -= GRAVITY * delta_time;
	}

	Vector3 movement = (direction * total_spd + Vector3::UP * (grounded ? (ground_y - getPosition().y) * 40 : v_spd)) * delta_time;
	move(movement);

	if (dashing) {
		if (current_animation != DASH) {
			last_animation = current_animation;
			current_animation = DASH;
			timer_anim = Game::instance->time;
		}
	}
	else if (total_spd > 0.3) {
		if (current_animation != WALKING) {
			last_animation = current_animation;
			current_animation = WALKING;
			timer_anim = Game::instance->time;
		}
	}
	else {
		if (current_animation != IDLE) {
			last_animation = current_animation;
			current_animation = IDLE;
			timer_anim = Game::instance->time;
		}
	}
	std::cout << grounded << " ";
//	move(direction * speed * delta_time);
//	if (!grounded)
//		move(Vector3::UP * v_spd * delta_time);
//	else
//		move(Vector3::UP * (ground_y - getPosition().y) * delta_time * 20);
	this->sphere_bullet_collision(player_center, HITBOX_RAD);
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