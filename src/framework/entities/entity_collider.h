#pragma once

#include "entity_mesh.h"
#include "framework/extra/coldet/coldet.h"

class EntityCollider : public EntityMesh {

	void getCollisionsWithModel(const Matrix44 model, const Vector3& center, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter);

	
	int layer = eCollisionFilter::ALL;

public:
	bool is_static = true;

	EntityCollider() {};
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") :
		EntityMesh(mesh, material, name) {};
	~EntityCollider() {};

	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter);


	int getLayer() { return layer; }
	void setLayer(int new_layer) { layer = new_layer; }
};
