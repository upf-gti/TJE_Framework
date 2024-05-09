#include "entityMesh.h"
#include "framework/camera.h"
#include "game/game.h"

#include <algorithm>


void EntityMesh::render(Camera* camera) {
	if (!mesh) {
		std::cout << "no mesh";
		return;
	}
	std::vector<Matrix44>* final_models = &models;
	std::vector<Matrix44> models_instanced;
	if (isInstanced) {
		for (int i = 0; i < models.size(); i++) {
			Vector3 center_world = models[i] * mesh->box.center;
			float aabb_radius = mesh->radius;
			if (camera->testSphereInFrustum(center_world, (1 + (1 / aabb_radius)) * aabb_radius)) {
				models_instanced.push_back(models[i]);
			}
		}
		final_models = &models_instanced;
	}
	else {
		Vector3 center_world = model * mesh->box.center;
		float aabb_radius = mesh->box.halfsize.length();

		if (!camera->testSphereInFrustum(center_world, aabb_radius)) {
			return;
		}
	}


	// Set flags
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	
	//// Get the last camera that was activated 
	//Camera* camera = Camera::current;

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
	}

	material.shader->enable();
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	}

	if (!isInstanced) {
		material.shader->setUniform("u_model", model);
	}

	material.shader->setUniform("u_time", Game::instance->time);

	if (isInstanced)
		mesh->renderInstanced(GL_TRIANGLES, final_models->data(), final_models->size());
	else
		mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();

	// Render hijos
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->render(camera);
	}
	// Or just update the father one
	// Entity::render(camera);
};


void EntityMesh::update(float delta_time) {
	Entity::update(delta_time);
}

void EntityMesh::addLOD(sMeshLOD mesh_lod) {
	mesh_lods.push_back(mesh_lod);
	std::sort(mesh_lods.begin(), mesh_lods.end(), [](sMeshLOD a, sMeshLOD b) { return a.distance < b.distance; });
}