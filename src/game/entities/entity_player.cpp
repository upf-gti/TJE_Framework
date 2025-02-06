#include "entity_player.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "framework/camera.h"
#include "framework/input.h"
#include "framework/entities/entity_collider.h"

#include "game/game.h"
#include "game/world.h"

EntityPlayer::EntityPlayer()
{
}

EntityPlayer::EntityPlayer(Mesh* mesh, const Material& material, const std::string& name)
{
	this->mesh = mesh;
	this->material = material;
	this->name = name;
}

void EntityPlayer::render(Camera* camera)
{
	//Render mesh
	EntityMesh::render(camera);
	/*float sphere_radius = World::get_instance()->sphere_radius;
	float sphere_ground_radius = World::get_instance()->sphere_ground_radius;
	float player_height = World::get_instance()->player_height;

	Shader* sphere_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.vs");
	Mesh* sphere_mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 sphere_m = model;

	sphere_shader->enable();

	//first sphere
	{
		sphere_m.translate(0.0f, sphere_ground_radius, 0.0f);
		sphere_m.scale(sphere_ground_radius, sphere_ground_radius, sphere_ground_radius);

		sphere_shader->setUniform("u_color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		sphere_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		sphere_shader->setUniform("u_model", sphere_m);

		sphere_mesh->render(GL_LINES);
	}

	//second Sphere
	{
		Matrix44 sphere_m = model;
		sphere_m.translate(0.0f, player_height, 0.0f);
		sphere_m.scale(sphere_radius, sphere_radius, sphere_radius);

		sphere_shader->setUniform("u_color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		sphere_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		sphere_shader->setUniform("u_model", sphere_m);

		sphere_mesh->render(GL_LINES);
	}

	sphere_shader->disable();*/
	
}

void EntityPlayer::update(float seconds_elapsed)
{
	float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

	Vector3 front = mYaw.frontVector(); //Vector3(0, 0, -1);
	Vector3 left = mYaw.rightVector(); //Vector3(1, 0, 0);

	Vector3 position = model.getTranslation();

	Vector3 move_dir;

	//moviment endavant/enrere

	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))
		move_dir += front;
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
		move_dir -= front;

	//moviement esquerra/dreta

	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT))
		move_dir -= left;
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT))
		move_dir += left;

	float speed_mult = walk_speed;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
		speed_mult *= 3.0f;

	move_dir.normalize();
	move_dir *= speed_mult;

	velocity += move_dir;

	//Check collisions with world entities

	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground_collisions;

	for (auto entity : World::get_instance()->root.children) {

		EntityCollider* ec = dynamic_cast<EntityCollider*>(entity);
		if (ec != nullptr)
			ec->getCollisions(position + velocity * seconds_elapsed, collisions, ground_collisions, ec->getLayer());
	}

	//Enviornment collisions
	for (const sCollisionData& collision : collisions) {
		//Move along wall when colliding
		Vector3 newDir = velocity.dot(collision.colNormal) * collision.colNormal;
		velocity.x -= newDir.x;
		velocity.y -= newDir.y;
		velocity.z -= newDir.z;
	}
	
	//Ground collisions
	bool is_grounded = false;

	for (const sCollisionData& collision : ground_collisions) {
		//If normal is pointing upwards, it means it's a floor collision
		float up_factor = fabsf(collision.colNormal.dot(Vector3::UP));
		if (up_factor > 0.8) {
			is_grounded = true;
		}
		if (collision.colPoint.y > (position.y * velocity.y * seconds_elapsed)) {
			position.y = collision.colPoint.y;
		}
	}

	//Gravity for falling
	if (!is_grounded) {
		velocity.y -= 0.9f * seconds_elapsed;
	}
	else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		velocity.y = 2.0f;
	}

	//Update player's position
	position += velocity * seconds_elapsed;

	//Decreasing velocity when not moving
	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	model.setTranslation(position);
	model.rotate(camera_yaw, Vector3(0, 1, 0)); //S'ha de provar per veure que fa

	EntityMesh::update(seconds_elapsed);
}

