#include "stage.h"
#include "game/game.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/camera.h"
#include "graphics/texture.h"
#include "framework/utils.h"
#include "game/entities/entity_player.h"
#include "game/world.h"
#include "framework/entities/entity_collider.h"
#include "framework/entities/entity_enemy.h"

#include <iomanip>
#include <sstream>

StageManager* StageManager::instance = nullptr;

void StageManager::render() { 
	if (current) 
		current->render(); 
}

//PLAY STAGE
PlayStage::PlayStage()
{
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;
}

void PlayStage::onEnter(Stage* previousStage)
{
	// lock mouse
	SDL_ShowCursor(false);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	Game::instance->mouse_locked = true;
}

void PlayStage::render()
{
	World::get_instance()->render();
}

void PlayStage::update(float seconds_elapsed)
{
	World::get_instance()->update(seconds_elapsed);

	Camera* camera = World::get_instance()->camera;

	//Get Ray direction

	Vector2 mouse_pos = Input::mouse_position;
	Vector3 ray_origin = camera->eye;

	Vector3 ray_direction = camera->getRayDirection(mouse_pos.x, mouse_pos.y, Game::instance->window_width, Game::instance->window_height);

	//Fill Collision vector

	std::vector<Vector3> collisions;

	for (Entity* e : World::get_instance()->root.children) {
		EntityCollider* collider = dynamic_cast<EntityCollider*>(e);
		if (!collider) {
			continue;
		}

		Vector3 col_point;
		Vector3 col_normal;

		if (collider->mesh->testRayCollision(collider->model, ray_origin, ray_direction, col_point, col_normal)); {
			collisions.push_back(col_point);
		}
	}

	//Get entities

	for (auto& col_point : collisions) {
		Mesh* mesh = Mesh::Get("");
		EntityMesh* new_entity = new EntityMesh(mesh, {});
		new_entity->model.setTranslation(col_point);
		World::get_instance()->addEntity(new_entity);
	}
		
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
	
}

void PlayStage::onResize(int width, int height)
{
	World::get_instance()->camera->aspect = width / (float)height;
	std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	Game::instance->window_width = width;
	Game::instance->window_height = height;
}