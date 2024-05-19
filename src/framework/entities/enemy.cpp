#include "enemy.h"
#include <game/stage.h>
#include "bullet/bullet.h"
#include "player.h"

void Enemy::render(Camera* camera)
{
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
				if (bullet->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal)) {
					colliding = true;
					bullet->to_delete = true;
					this->currHP -= bullet->damage;
				}
			}
		}
		else {
			if (bullet->mesh->testSphereCollision(bullet->model, position, radius, data.colPoint, data.colNormal)) {
				colliding = true;
				bullet->to_delete = true;
				this->currHP -= bullet->damage;
			}
		}
	}
}

void Enemy::update(float time_elapsed)
{
	//std::vector<sCollisionData> collisions;
	//std::vector<sCollisionData> ground;

	//Vector3 player_center = getPosition() + Vector3(0, player_height, 0);

	//colliding = Stage::instance->sphere_collided(collisions, player_center, HITBOX_RAD);
	//Stage::instance->ray_collided(ground, player_center, -Vector3::UP, 1000, FLOOR);

	//for (sCollisionData& g : collisions) {
	//	direction += g.colNormal * 10000;
	//	direction.y = 0;
	//}
	//direction.normalize();
	//ground_below_y = 10000;
	//ground_y = -10000;

	//touching_ground = false;

	//for (sCollisionData& g : ground) {
	//	if (ground_y < g.colPoint.y) {
	//		ground_y = g.colPoint.y;
	//		ground_below_y = player_center.y - g.colPoint.y;
	//		ground_normal = g.colNormal;
	//	}
	//	if (player_center.y - g.colPoint.y < player_height + 0.01 && player_center.y - g.colPoint.y > player_height - 0.3) {
	//		touching_ground = true;
	//		v_spd = g.colNormal.y * ground_below_y * ground_below_y;
	//	}
	//}
	//grounded = touching_ground;

	Vector3 player_center = getPosition();
	player_center.y += PLAYER_HEIGHT;

	ground_below_y = 10000;
	ground_y = -10000;

	std::vector<sCollisionData> ground;
	Stage::instance->ray_collided(ground, player_center, -Vector3::UP, 1000, FLOOR);

	for (sCollisionData& g : ground) {
		if (ground_y < g.colPoint.y) {
			ground_y = g.colPoint.y;
			ground_below_y = player_center.y - g.colPoint.y;
			ground_normal = g.colNormal;
		}
		if (player_center.y - g.colPoint.y < PLAYER_HEIGHT + 0.01 && player_center.y - g.colPoint.y > PLAYER_HEIGHT - 0.3) {
			touching_ground = true;
			//v_spd = g.colNormal.y * ground_below_y * ground_below_y;
		}
	}

	this->sphere_bullet_collision(player_center, HITBOX_RAD);
}