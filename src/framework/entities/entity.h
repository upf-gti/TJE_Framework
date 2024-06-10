#pragma once

#include "framework/includes.h"
#include "framework/framework.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"



class Camera;

//#define N_TYPES 32
//struct EntityType {
//
//	// Attributes of the derived class  
//	Mesh* mesh = nullptr;
//	Texture* texture = nullptr;
//	Shader* shader = nullptr;
//	Vector4 color;
//};
//
//// Array of types (max 32 types)
//EntityType entityTypes[N_TYPES];

class Entity {

public:

	// Index to the array (DOP)
	int type;

	Entity() {}; 			// Constructor
	virtual ~Entity() {}; 	// Destructor

	std::string name;

	Matrix44 model;

	Entity* parent = nullptr;
	std::vector<Entity*> children;

	void addChild(Entity* child);
	void removeChild(Entity* child);

	// Methods that should be overwritten
	// by derived classes 
	virtual void render(Camera* camera);
	virtual void renderWithLights(Camera* camera);
	virtual void update(float delta_time);

	Vector3 nullify_coords(Vector3 v, uint8 axis) {
		switch (axis) {
		case 1:
			return v * Vector3(0, 1, 1);
		case 2:
			return v * Vector3(1, 0, 1);
		case 3:
			return v * Vector3(1, 1, 0);
		default: 
			return v;
		}
	}

	// Some useful methods
	Matrix44 getGlobalMatrix();
	float distance(Entity* e);
};
