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
			float max = model._11 > model._22 ? model._11 : model._22;
			max = max > model._33 ? max : model._33;
			float sphere_radius = mesh->radius;
			Vector3 center_world = models[i] * mesh->box.center;
			if (camera->testSphereInFrustum(center_world, sphere_radius) && camera->eye.distance(center_world) > 2) {
				models_instanced.push_back(models[i]);
			}
		}
		final_models = &models_instanced;
	}
	else {
		Vector3 center_world = model * mesh->box.center;
		float sphere_radius = mesh->box.halfsize.length();
		if (!camera->testSphereInFrustum(center_world, sphere_radius * 5)) {
			return;
		}
		int is_wall = (type & WALL);
		int is_border = (type & BORDER);
		center_world.y = camera->eye.y;
		float dist = camera->eye.distance(center_world);
		if (dist < 7 && (is_wall != 0)) material.color.w = clamp(((dist - 2)/ 5), 0, 1);
		else if (dist < 30 && (is_border != 0)) { 
			material.color.w = clamp(((dist - 25) / 5), .2, 1); 
		}
		else material.color.w = 1;
	}


	// Set flags
	if (material.color.w != 1) {
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}
	
	//// Get the last camera that was activated 
	//Camera* camera = Camera::current;



	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs");
		if (isAnimated) material.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	}
	if (isAnimated) 	anim->assignTime(Game::instance->time);

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
	else if (isAnimated) {
		mesh->renderAnimated(GL_TRIANGLES, &anim->skeleton);
		std::cout << isAnimated << std::endl;
	}
	else mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material.shader->disable();

	if (material.color.w != 1) {
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

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