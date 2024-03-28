#pragma once

#include "framework/includes.h"
#include "framework/framework.h"

class Shader;
class Texture;

class Material {

	Shader* shader = nullptr;
	Vector4 color = Vector4(1.f);

	Texture* diffuse = nullptr;
	Texture* normals = nullptr;

	Vector3 Kd = Vector3(1.0f);
	Vector3 Ks = Vector3(0.2f);

	bool transparent = false;

	float tiling = 1.f;
};

struct sRenderData {
	Material material;
	std::vector<Matrix44> models;
};