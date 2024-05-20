#include "enemy.h"
#include <game/stage.h>
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
	for (Bullet* bullet : Stage::instance->player->bullets) {
		sCollisionData data;

		if (bullet->isInstanced) {
			for (Matrix44& instanced_model : bullet->models) {
				if (bullet->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal) && bullet->fromPlayer) {
					colliding = true;
					bullet->to_delete = true;
					this->currHP -= bullet->damage;
				}
			}
		}
		else {
			if (bullet->mesh->testSphereCollision(bullet->model, position, radius, data.colPoint, data.colNormal) && bullet->fromPlayer) {
				colliding = true;
				bullet->to_delete = true;
				this->currHP -= bullet->damage;
			}
		}
	}
}

void Enemy::update(float time_elapsed)
{
	Vector3 player_center = getPosition();
	Vector3 pPosition = Stage::instance->player->getPosition();

	//TODO: fix enemy pointing at player
	this->model.setFrontAndOrthonormalize(pPosition - player_center);
	player_center.y += PLAYER_HEIGHT;

	ground_below_y = 10000;
	ground_y = -10000;

	std::vector<sCollisionData> ground;
	std::vector<sCollisionData> collisions;
	Stage::instance->ray_collided(ground, player_center, -Vector3::UP, 1000, FLOOR);
	Stage::instance->sphere_collided(collisions, player_center, HITBOX_RAD, COL_TYPE::ENEMCOLS);

	Vector3 currDirection = direction;

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
		if (startMoving + 3 <= Game::instance->time) {
			moving = false;
			startFiring = Game::instance->time;
		}
	}
	else
	{
		if (startFiring + 2 <= Game::instance->time)
		{
			moving = true;
			startMoving = Game::instance->time;
			direction.x = (rand() % 2) * 2 - 1;
			direction.z = (rand() % 2) * 2 - 1;
		}
		//if (bulletCD - 0.5 >= Game::instance->time) {
			//bulletCD = Game::instance->time;
			Patterns::autoAim(Stage::instance->player->getPosition()+Vector3(0, 0.5, 0), Vector3(0, 0, 1), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
		//}
	}

	for (int i = 0; i < bullets.size(); i++) {
		Bullet* b = bullets[i];
		if (Game::instance->time - b->timer_spawn > 5 || b->to_delete) {
			bullets.erase((bullets.begin() + i));
			delete b;
		}
		else b->update(time_elapsed);
	}
	Entity::update(time_elapsed);

	this->sphere_bullet_collision(player_center, HITBOX_RAD);
}