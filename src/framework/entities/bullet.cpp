#include "bullet.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

// cubemap


Vector3 Bullet::getPosition() {
	return model.getTranslation();
}

void Bullet::render(Camera* camera) {
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

	// Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");

	// Enable shader and pass uniforms 
	material.shader->enable();
	material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	}

	//if (!isInstanced) {
	//	material.shader->setUniform("u_model", model);
	//}
	//if (isInstanced)
	//	mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	//else
	//	// Render the mesh using the shader
	//	mesh->render(GL_TRIANGLES);

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_time", Game::instance->time);
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

void Bullet::addInstance(const Matrix44& model)
{
}

void Bullet::move(Vector3 vec) {
	model.translate(vec);

}

void Bullet::update(float delta_time) {
	if (has_objective) {
		Vector3 change_direction = model.getTranslation() - objective;
		direction += change_direction.normalize() * 0.5;
	}
	move(speed * delta_time * direction);

	Entity::update(delta_time);
}



void Bullet::onMouseWheel(SDL_MouseWheelEvent event)
{

}

void Bullet::onMouseButtonDown(SDL_MouseButtonEvent event)
{

}

void Bullet::onKeyUp(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_SPACE)
	{
	}
}