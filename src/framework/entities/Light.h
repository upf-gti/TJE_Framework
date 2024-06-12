#pragma once
#include "framework/includes.h"
#include "framework/framework.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "framework/entities/entity.h"

enum eLightType : uint32 {
	NO_LIGHT = 0,
	POINTLIGHT = 1,
	SPOT = 2,
	DIRECTIONAL = 3
};

class Light : Entity
{
public:
	eLightType lightType;

	Matrix44 model;

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

	Light(eLightType type, bool isDay=false);

	Vector3 getFront() { return model.frontVector(); }
};

