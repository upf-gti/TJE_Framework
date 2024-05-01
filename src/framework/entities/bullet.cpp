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
	// Entity::render(camera);
};

void Bullet::addInstance(const Matrix44& model)
{
}

void Bullet::move(Vector3 vec) {
	model.translate(vec);

}

void Bullet::update(float delta_time) {
	if (has_objective) {
		//Vector3 this_pos_0 = nullify_coords(model.getTranslation() - objective, 2);
		//Vector3 front_vec_0 = nullify_coords(model.frontVector(), 2);
		//Vector3 objective_0 = nullify_coords(objective, 2);

		//Vector3 a = this_pos_0 - front_vec_0;
		//Vector3 b = objective_0 - front_vec_0;

		//this_pos_0 = this_pos_0.normalize();

		//float azimuth = acos(clamp(front_vec_0.dot(this_pos_0), -0.999f, 0.999f));
		//if (a.cross(b).y > 0) azimuth *= -1; // We compute the cross product to check to which direction to make the rotation. Check https://math.stackexchange.com/questions/1232773/is-the-point-on-the-left-or-the-right-of-the-vector-in-2d-space
		//model.rotate(azimuth * 10 * delta_time, model.topVector());

		//float distance = model.getTranslation().distance(objective);

		//float elevation = asin(clamp((model.getTranslation().y - objective.y) / distance, -0.99999f, 0.99999f));
		//elevation > 0 ? elevation = (PI / 2) - elevation : elevation = - ((PI / 2) + elevation);
		
		// std::cout << model.getYawRotationToAimTo(objective) * 360 / (2 * PI) << std::endl;

	/*	model.rotate(delta_time * model.getYawRotationToAimTo(Vector3(0,0,0)), model.rightVector());*/

		model.rotate(model.getYawRotationToAimTo(objective) * delta_time * 3, Vector3(0, 1, 0));
		model.rotate(model.getPitchRotationToAimTo(objective) * delta_time * 3, Vector3(1, 0, 0));

	}
	move(speed * delta_time * direction);



	// Entity::update(delta_time);
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