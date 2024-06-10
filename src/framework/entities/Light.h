#pragma once
#include "framework/includes.h"
#include "framework/framework.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "framework/entities/entity.h"

class Light : Entity
{
public:
	float intensity;
	Vector3 color;
	float near_distance;
	float max_distance;
	bool cast_shadows;
	float shadow_bias;
	Vector2 cone_info;
	float area; //for direct;

	FBO* shadowMapFBO;
	mat4 shadowMap_viewProjection;

	Light();

	Vector3 getFront() { return model.frontVector(); }
};

