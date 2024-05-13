#include "entity.h"
#include "entity_mesh.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/material.h"
#include "framework/camera.h"

// Methods overwritten from base class

void EntityMesh::render(Camera* camera)
{
	if (!mesh) return;

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/");
	}

	//Enable shader
	material.shader->enable();

	//Upload Uniforms
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	if (material.diffuse) {
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}

	if (isInstanced)
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	else
		mesh->render(GL_TRIANGLES);

	//Disable shader
	material.shader->disable();

	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void EntityMesh::update(float delta_time)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->update(delta_time);
	}
}

void EntityMesh::addInstance(const Matrix44& model)
{
	models.push_back(model);
}
