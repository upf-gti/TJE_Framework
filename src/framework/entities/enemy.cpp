#include "enemy.h"
#include <game/StageManager.h>
#include "bullet/bullet.h"
#include "player.h"

Enemy::Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP)
	: EntityCollider(mesh, material, ENEMY, name, is_dynamic)
{
	loadTextures();

	this->maxHP = HP; this->currHP = HP;
	this->type = COL_TYPE::ENEMY;

	moving = true;
	startMoving = Game::instance->time;
	startFiring = -1;

	direction = Vector3(0.0f);

	m_spd = 10;
	v_spd = 0;

	direction.x = (rand() % 2) * 2 - 1;
	direction.z = (rand() % 2) * 2 - 1;
}

void Enemy::render(Camera* camera)
{
	for (int i = bullets.size() - 1; i >= 0; i--)
		bullets[i]->render(camera);
	bullets_normal.render(camera);
	EntityMesh::render(camera);
	showHitbox(camera);
}

void Enemy::showHitbox(Camera* camera) {


	Matrix44 m = model;

	flat_shader->enable();

	float sphere_radius = HITBOX_RAD;
	m.translate(0.0f, PLAYER_HEIGHT, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	flat_shader->setUniform("u_color", Vector4(touching_ground, 0.0f, colliding, 1.0f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", m);

	hitbox_mesh->render(GL_LINES);

	flat_shader->disable();
}

void Enemy::sphere_bullet_collision(Vector3 position, float radius) {
	Stage* stage = StageManager::instance->currStage;
	for (Bullet* bullet : StageManager::instance->currStage->player->bullets) {
		sCollisionData data;
		if (bullet->isInstanced) {
			for (Matrix44& instanced_model : bullet->models) {
				if (bullet->mesh->testSphereCollision(instanced_model, position, 3*radius, data.colPoint, data.colNormal)) {
					colliding = true;
					bullet->to_delete = true;
					this->currHP -= bullet->damage;
				}
			}
		}
		else {
			if (bullet->mesh->testSphereCollision(bullet->model, position, 3 * radius, data.colPoint, data.colNormal)) {
				colliding = true;
				bullet->to_delete = true;
				this->currHP -= bullet->damage;
			}
		}
	}
	BulletNormal& bns = stage->player->bullets_normal;
	for (int i = 0; i < bns.models.size(); i++) {
		Matrix44& m = stage->player->bullets_normal.models[i];
		sCollisionData data;
		if (bns.mesh->testSphereCollision(m, position, 3 * radius, data.colPoint, data.colNormal)) {
			bns.models.erase((bns.models.begin() + i));
			bns.speeds.erase((bns.speeds.begin() + i));
			this->currHP -= bns.damage;
		}
	}
	BulletAuto& bas = stage->player->bullets_auto;
	for (int i = 0; i < bas.models.size(); i++) {
		Matrix44& m = bas.models[i];
		sCollisionData data;
		if (bas.mesh->testSphereCollision(m, position, 2 * radius, data.colPoint, data.colNormal)) {
			bas.models.erase((bas.models.begin() + i));
			bas.speeds.erase((bas.speeds.begin() + i));
			this->currHP -= bas.damage;
		}
	}
}

void Enemy::update(float time_elapsed)
{
	Stage* stage = StageManager::instance->currStage;
	Vector3 player_center = getPosition();

	if (looking_at_player) {

		Vector3 pPosition = stage->player->getPosition();

		Vector3 front = pPosition - player_center;
		front.y = 0;

		//TODO: fix enemy pointing at player
		this->model.setFrontAndOrthonormalize(front);
	}

	player_center.y += PLAYER_HEIGHT;

	ground_below_y = 10000;
	ground_y = -10000;

	std::vector<sCollisionData> ground;
	std::vector<sCollisionData> collisions;
	stage->ray_collided(stage->root, ground, player_center, -Vector3::UP, 1000, FLOOR);
	stage->sphere_collided(stage->root, collisions, player_center, HITBOX_RAD, COL_TYPE::EBCOLS);


	Vector3 currDirection = direction;

	if (Input::isKeyPressed(SDL_SCANCODE_K))
		Patterns::circle(StageManager::instance->currStage->player->getPosition() + Vector3(0, 0.5, 0), Vector3(0, 0, 1), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);


	for (sCollisionData& g : collisions) {
		currDirection += g.colNormal;
		currDirection.y = 0;
	}

	for (sCollisionData& g : ground) {
		if (ground_y < g.colPoint.y) {
			ground_y = g.colPoint.y;
			ground_below_y = player_center.y - g.colPoint.y;
			ground_normal = g.colNormal;
		}
		if (player_center.y - g.colPoint.y < PLAYER_HEIGHT + 0.01 && player_center.y - g.colPoint.y > PLAYER_HEIGHT - 0.3) {
			touching_ground = true;
			v_spd = g.colNormal.y * ground_below_y * ground_below_y;
		}
	}

	grounded = touching_ground;

	if (!grounded) {
		v_spd -= GRAVITY * time_elapsed;
	}

	Vector3 movement = (currDirection.normalize() + Vector3::UP * (grounded ? (ground_y - getPosition().y) * 40 : v_spd)) * time_elapsed;

	if (moving)
	{
		this->model.translateGlobal(movement);
		if (startMoving + 2 <= Game::instance->time) {
			moving = false;
			startFiring = Game::instance->time;
			float r = random() * 3;
			current_pattern = (pattern) clamp(floor(r), 0, 2);
			//current_pattern = SWIRL;
			std::cout << current_pattern << " " << r << std::endl;
		}
	}
	else
	{
		switch (current_pattern) {
		case SWIRL:
			if (startFiring + 4 <= Game::instance->time)
			{
				moving = true;
				startMoving = Game::instance->time;
				direction.x = (rand() % 2) * 2 - 1;
				direction.z = (rand() % 2) * 2 - 1;
			}
			if (bulletCD + 0.1 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m = model;
				_m.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Patterns::circle2(_m, bullets_normal, 6);
			}
			break;
		case SHOTGUN:
			if (startFiring + 1 > Game::instance->time) {
				if (bulletCD + 0.05 <= Game::instance->time) {
					bulletCD = Game::instance->time;
					Matrix44 _m = model;
					_m.rotate(-PI / 12, Vector3::UP);
					for (int i = -1; i <= 1; ++i) {
						Patterns::circle2(_m, bullets_normal);
						_m.rotate(PI / 12, Vector3::UP);
					}
				}
			}
			else if ((startFiring + 2.5 > Game::instance->time && startFiring + 1.5 <= Game::instance->time)) {
				if (bulletCD + 0.05 <= Game::instance->time) {
					bulletCD = Game::instance->time;
					Matrix44 _m = model;
					_m.rotate(-3 * PI / 24, Vector3::UP);
					for (int i = -1; i <= 2; ++i) {
						Patterns::circle2(_m, bullets_normal);
						_m.rotate(PI / 12, Vector3::UP);
					}
				}
			}
			else if ((startFiring + 3.4 > Game::instance->time && startFiring + 2.7 <= Game::instance->time)) {
				if (bulletCD + 0.05 <= Game::instance->time) {
					bulletCD = Game::instance->time;
					Matrix44 _m = model;
					_m.rotate(-PI / 12, Vector3::UP);
					for (int i = -1; i <= 1; ++i) {
						Patterns::circle2(_m, bullets_normal);
						_m.rotate(PI / 12, Vector3::UP);
					}
				}
			}
			else if ((startFiring + 5 > Game::instance->time && startFiring + 3.7 <= Game::instance->time)) {
				if (bulletCD + 0.05 <= Game::instance->time) {
					bulletCD = Game::instance->time;
					Matrix44 _m = model;
					_m.rotate(-3 * PI / 24, Vector3::UP);
					for (int i = -1; i <= 2; ++i) {
						Patterns::circle2(_m, bullets_normal);
						_m.rotate(PI / 12, Vector3::UP);
					}
				}
			}
			else if (startFiring + 6 <= Game::instance->time)
			{
				moving = true;
				startMoving = Game::instance->time;
				direction.x = (rand() % 2) * 2 - 1;
				direction.z = (rand() % 2) * 2 - 1;
			}
			break;
		case HORIZONTAL:
			if (startFiring + 4 <= Game::instance->time)
			{
				moving = true;
				startMoving = Game::instance->time;
				direction.x = (rand() % 2) * 2 - 1;
				direction.z = (rand() % 2) * 2 - 1;
			}
			if (bulletCD + 0.3 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m = model;
				//_m.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Patterns::horizontal2(_m, bullets_normal);
			}
			break;
		}
	}

	for (int i = 0; i < bullets.size(); i++) {
		Bullet* b = bullets[i];
		if (Game::instance->time - b->timer_spawn > 5 || b->to_delete) {
			bullets.erase((bullets.begin() + i));
			delete b;
		}
		else b->update(time_elapsed);
	}

	bullets_normal.update(time_elapsed);

	this->sphere_bullet_collision(player_center, HITBOX_RAD);
}