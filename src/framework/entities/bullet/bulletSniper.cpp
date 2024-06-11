#include "bulletSniper.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>

// cubemap


Vector3 BulletSniper::getPosition() {
	return model.getTranslation();
}

void BulletSniper::drawHitBox(Camera* camera) {
	// Disable shader after finishing rendering
	material.shader->disable();
	glDisable(GL_BLEND);

	Matrix44 m = model_base;

	material.shader->enable();
	m.setTranslation(model.getTranslation() + model_base.frontVector() * (scale / 10));
	float sphere_radius = 0.2;
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	material.shader->setUniform("u_color", Vector4(0.f, 1.0f, 0.f, 1.0f));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_model", m);

	hitbox_mesh->render(GL_LINES);

	material.shader->disable();
}

void BulletSniper::render(Camera* camera) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/basic.vs" : "data/shaders/basic.vs");
	}

	material.shader->enable();

	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_time", Game::instance->time);

	mesh->render(GL_TRIANGLES);

	drawHitBox(camera);
};


void BulletSniper::move(Vector3 vec) {
	model.translate(vec);
}

void BulletSniper::update(float delta_time) {
	Stage* stage = StageManager::instance->currStage;
	std::vector<sCollisionData> collisions;
	if (active) {
		acceleration += 2 * delta_time;
		speed += acceleration;
		scale += speed * delta_time;
		model = model_base;
		model.scale(0.5, 0.5, scale / 10);
		model.translateGlobal(model_base.frontVector() * (scale / 10));
		Vector3 bullet_center = model.getTranslation() + model_base.frontVector() * (scale / 10);
		bool colliding = stage->sphere_collided(stage->root, collisions, bullet_center, 0.2, SCENARIO);
		if (colliding) active = false;
	}
	else despawning(delta_time);
	//move(Vector3(0, 0, speed * delta_time));
}



void BulletSniper::onMouseWheel(SDL_MouseWheelEvent event)
{

}

void BulletSniper::onMouseButtonDown(SDL_MouseButtonEvent event)
{

}

void BulletSniper::onKeyUp(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_SPACE)
	{
	}
}