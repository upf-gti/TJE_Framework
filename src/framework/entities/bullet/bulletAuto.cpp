#include "bulletAuto.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>

// cubemap


Vector3 BulletAuto::getPosition() {
	return model.getTranslation();
}

void BulletAuto::render(Camera* camera) {
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//if (!material.shader) {
	//	material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
	//}

	//material.shader->enable();

	//material.shader->setUniform("u_color", material.color);
	//material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	//material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	//material.shader->setUniform("u_model", model);
	//material.shader->setUniform("u_time", Game::instance->time);

	//mesh->render(GL_TRIANGLES);

	//// Disable shader after finishing rendering
	//material.shader->disable();
	//glDisable(GL_BLEND);
	EntityMesh::render(camera);
};


void BulletAuto::move(Vector3 vec) {
	model.translate(vec);
}

void BulletAuto::update(float delta_time) {
	Stage* stage = StageManager::instance->currStage;
	if (isInstanced) {
		for (int i = 0; i < models.size(); i++) {
			std::vector<sCollisionData> collisions;
			Matrix44& m = models[i];
			Vector3 bullet_center = m.getTranslation();

			int a = SCENARIO;
			bool colliding = stage->sphere_collided(collisions, bullet_center, 0.05, (COL_TYPE)a);
			if (colliding) {
				// stage->root_transparent->addChild((Entity*) new BulletNormal(this->mesh, this->material, this->direction, m, 0));
				models.erase((models.begin() + i));
				speeds.erase((speeds.begin() + i));
				collisions.clear();
				continue;
			};
			Vector3 vec = BULLET_SPD * Vector3(0, 0, 1) * delta_time;
			m.rotate(model.getYawRotationToAimTo(objective) * delta_time * 3, Vector3(0, 1, 0));
			m.rotate(model.getPitchRotationToAimTo(objective) * delta_time * 3, Vector3(1, 0, 0));
			m.translate(vec);
			collisions.clear();
		}
	}
	else {
		std::vector<sCollisionData> collisions;
		if (active) {
			Vector3 bullet_center = model.getTranslation();
			bool colliding = stage->sphere_collided(collisions, bullet_center, 0.05);
			if (colliding) to_delete = true;
			model.rotate(model.getYawRotationToAimTo(objective) * delta_time * 3, Vector3(0, 1, 0));
			model.rotate(model.getPitchRotationToAimTo(objective) * delta_time * 3, Vector3(1, 0, 0));
			move(speed * delta_time * direction);
		}
		else despawning(delta_time);
	}
}



void BulletAuto::onMouseWheel(SDL_MouseWheelEvent event)
{

}

void BulletAuto::onMouseButtonDown(SDL_MouseButtonEvent event)
{

}

void BulletAuto::onKeyUp(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_SPACE)
	{
	}
}