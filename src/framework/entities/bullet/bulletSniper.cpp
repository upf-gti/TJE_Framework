#include "bulletSniper.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

// cubemap


Vector3 BulletSniper::getPosition() {
	return model.getTranslation();
}

void BulletSniper::render(Camera* camera) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
	}

	material.shader->enable();

	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_time", Game::instance->time);

	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();
	glDisable(GL_BLEND);
};


void BulletSniper::move(Vector3 vec) {
	model.translate(vec);
}

void BulletSniper::update(float delta_time) {
	scale += 20 * delta_time;
	Matrix44 m = model;
	model.scale(1, 1, scale);
	//if (material.color.w > 0)
	//	material.color.w -= opacity_dec * delta_time;
	//else material.color.w = 0;
	//move(Vector3(20 * delta_time, 0, 0));
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