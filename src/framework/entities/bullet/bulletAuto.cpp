#include "bulletAuto.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

// cubemap


Vector3 BulletAuto::getPosition() {
	return model.getTranslation();
}

void BulletAuto::render(Camera* camera) {
	EntityMesh::render(camera);
};


void BulletAuto::move(Vector3 vec) {
	model.translate(vec);
}

void BulletAuto::update(float delta_time) {
	model.rotate(model.getYawRotationToAimTo(objective) * delta_time * 3, Vector3(0, 1, 0));
	model.rotate(model.getPitchRotationToAimTo(objective) * delta_time * 3, Vector3(1, 0, 0));
	move(speed * delta_time * direction);
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