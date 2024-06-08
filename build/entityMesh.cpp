#include "entityMesh.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/material.h"
#include "graphics/shader.h"


/*EntityMesh::EntityMesh(Mesh* me, Texture* te, Material* ma, Shader* s, Vector4 c) {
	this->mesh = me;
	this->texture = te;
	this->material = ma;
	this->shader = s;
	this->color = c;
};


void EntityMesh::render() {
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	//this->material->enable(); TODO: Verify why we can't use material when we should be able to
	this->shader->enable();
	this->shader->setUniform("u_model", model);
	this->shader->setUniform("u_viewproj", camera->projection_matrix);
	//this->shader->setTexture("u_texture", texture );
	material.shader->enable();
	if (material.diffuse) {
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}
	material.shader->setUniform("u_model", getGlobalMatrix());

	// Render the mesh using the shader
	this->mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	this->shader->disable();
	
};

void EntityMesh::update(float elapsed_time) {

};

*/