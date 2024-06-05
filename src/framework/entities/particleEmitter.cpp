#include "ParticleEmitter.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>
#include <framework/input.h>




void ParticleEmitter::render(Camera* camera) {
	// Sort
	std::vector<Vector2> uv_offsets;
	for (int i = 0; i < texture_grid_size; i++)
		for (int j = 1; i <= texture_grid_size; j++)
			uv_offsets.push_back(Vector2(i / (float)texture_grid_size, 1.f - j / (float)texture_grid_size));

	Mesh quad;
	Vector3 right = camera->getLocalVector(Vector3(1,0,0)); //camera local vector right
	Vector3 top = camera->getLocalVector(Vector3::UP); // camera top

	for (sParticle& p : particles) {
		if (!p.active) continue;

		Vector3 pos = p.position;
		pos = model * pos;

		float nt = clamp(p.ttl / max_ttl, 0, 1);
		
		Vector4 color = sample<Vector4>(nt, colors.size(), &colors[0]);
		float size = sample<float>(nt, sizes.size(), &sizes[0]);

		int grid_frames = texture_grid_size * texture_grid_size;
		int cloud_index = animated_texture ? nt * grid_frames : p.id % grid_frames;
		float d_uvs = 1.f / texture_grid_size;
		Vector2 uv_offset = uv_offsets[cloud_index];


		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (- top - right) * size);
		quad.uvs.push_back(Vector2(0, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (- top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top - right) * size);
		quad.uvs.push_back(Vector2(0, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);

	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	material.shader->enable();

	Stage* stage = StageManager::instance->currStage;

	material.shader->setUniform("u_model", Matrix44());
	material.shader->setUniform("u_color", Vector4(1.0f));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);




	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0);
	}



};


void ParticleEmitter::update(float delta_time) {

}

void ParticleEmitter::emit() {
	int a = 1;
};
