#pragma once
#include "framework/entities/entity.h"
#include "framework/utils.h"
#include "framework/entities/entity_mesh.h"
#include "framework/entities/entity_enemy.h"

class Camera;
class EntityPlayer;

class World {

	static World* instance;

	public:

		static World* get_instance() {
			if (instance != nullptr) {
				return instance;
			}
			instance = new World();
			return instance;
		}

		World();

		Entity root;

		EntityPlayer* player = nullptr;

		EntityEnemy* enemy = nullptr;

		EntityMesh* skybox = nullptr;

		Camera* camera = nullptr;

		std::vector<Vector3> waypoints;

		float camera_yaw = 0.f;
		float camera_pitch = 0.f;
		float camera_speed = 2.0f;
		float mouse_speed = 0.5f;

		bool free_camera = false;

		void render();
		void update(float seconds_elapsed);

		float sphere_radius = 30.0f;
		float sphere_ground_radius = 30.0f;
		float player_height = 30.0f;

		//Scene

		std::vector<Entity*> entities_to_destroy;

		bool parseScene(const char* filename, Entity* root);
		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);

		void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter);

		sCollisionData raycast(const Vector3& origin, const Vector3& direction, int layer, float max_ray_dist);

};