#include "bulletNormal.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"

#include <algorithm>

// cubemap


Vector3 BulletNormal::getPosition() {
	return model.getTranslation();
}

void BulletNormal::render(Camera* camera) {
	EntityMesh::render(camera);
};


void BulletNormal::move(Vector3 vec) {
	model.translate(vec);
}

void BulletNormal::update(float delta_time) {
	speed += acceleration * delta_time;
	rotation_angle += rotation_angle_accel * delta_time;
	model.rotate(rotation_angle, Vector3(0, 1, 0));
	move(speed * delta_time * direction);
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