#include "bulletNormal.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>

// cubemap


Vector3 BulletNormal::getPosition() {
	return model.getTranslation();
}

void BulletNormal::render(Camera* camera) {
	//if (!mesh) {
	//	std::cout << "no mesh";
	//	return;
	//}
	//if (!material.shader) {
	//	material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
	//}

	//material.shader->enable();

	//material.shader->setUniform("u_color", material.color);
	//material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	//material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	//material.shader->setUniform("u_time", Game::instance->time);

	//if (isInstanced) {
	//	mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	//}
	//else {
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	material.shader->setUniform("u_model", model);
	//	mesh->render(GL_TRIANGLES);
	//	glDisable(GL_BLEND);
	//}
	//// Disable shader after finishing rendering
	//material.shader->disable();
	EntityMesh::render(camera);
};


void BulletNormal::move(Vector3 vec) {
	model.translate(vec);
}

void BulletNormal::update(float delta_time) {
	Stage* stage = StageManager::instance->currStage;
	std::vector<sCollisionData> collisions;
	if (isInstanced) {
		rotation_angle += rotation_angle_accel * delta_time * 10;
		for (int i = 0; i < models.size(); i++) {
			Matrix44& m = models[i];
			speeds[i] += accels[i] * delta_time;
			angular_speeds[i] += angular_accels[i] * delta_time;
			m.rotate(angular_speeds[i], Vector3::UP);
			Vector3 bullet_center = m.getTranslation();
			int a = SCENARIO;
			bool colliding = stage->sphere_collided(stage->root, collisions, bullet_center, 0.05, SCENARIO );
			if (colliding) {
				// stage->root_transparent->addChild((Entity*) new BulletNormal(this->mesh, this->material, this->direction, m, 0));
				despawnBullet(i);
				collisions.clear();
				continue;
			};
			m.rotate(rotation_angle, Vector3(0, 1, 0));
			time_since_spawn += delta_time;
			Vector3 vec = speeds[i] * Vector3(0, 0, 1) * delta_time;
			m.translate(vec);
			collisions.clear();
		}
	}
	else {
		if (active) {
			Vector3 bullet_center = model.getTranslation();
			bool colliding = stage->sphere_collided(stage->root, collisions, bullet_center, 0.05);
			if (colliding) to_delete = true;
			speed += acceleration * delta_time;
			rotation_angle += rotation_angle_accel * delta_time * 10;
			model.rotate(rotation_angle, Vector3(0, 1, 0));

			// Update time since spawn
			time_since_spawn += delta_time;

			//// Calculate the sine wave offset
			//float wave_amplitude = 0.5f; // Adjust the amplitude as needed
			//float wave_frequency = 5.0f; // Adjust the frequency as needed
			//float wave_offset = wave_amplitude * sin(time_since_spawn * wave_frequency);

			//// Apply the wave offset to the bullet's direction
			//Vector3 wave_direction = direction;
			//wave_direction.x += wave_offset; // Apply wave to x-direction, can also be y or z

			move(speed * Vector3(0, 0, 1) * delta_time /** wave_direction*/);
		}
		else {
			despawning(delta_time);
		}
	}
}



void BulletNormal::onMouseWheel(SDL_MouseWheelEvent event)
{

}

void BulletNormal::onMouseButtonDown(SDL_MouseButtonEvent event)
{

}

void BulletNormal::onKeyUp(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_SPACE)
	{
	}
}