#include "entityUI.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>
#include <framework/input.h>




void EntityUI::render(Camera* camera2D) {
	if (!visible) return;

	if (!is3d) glEnable(GL_DEPTH_TEST);
	
	if (!mesh) {
		std::cout << "NO MESH";
		return;
	}






	std::cout << "EntityUI ";


	if (posteffects) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	material.shader->enable();

	Stage* stage = StageManager::instance->currStage;

	//material.shader->setUniform("u_camera_pos")
	material.shader->setUniform("u_model", Matrix44());
	material.shader->setUniform("u_color", Vector4(1.0f));
	material.shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	//material.shader->setUniform("u_mask", mask);

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0);
	}

	if (is3d) {
		Vector2 _size = size;
		float max_dist = 5.0f;
		float dist = clamp(stage->camera->eye.distance(pos3d), 0.01f, max_dist);
		_size *= 1.f - (dist / max_dist);

		Mesh quad;
		quad.createQuad(position.x, position.y, _size.x, _size.y, true);
		quad.render(GL_TRIANGLES);
	}
	else {
		std::cout << "Paiting Quad!";
		mesh->render(GL_TRIANGLES);
	}
	material.shader->disable();


	if (posteffects) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}


	// Render hijos
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->render(camera2D);
	}
};


void EntityUI::update(float delta_time) {
	Vector2 mouse_pos = Input::mouse_position;

	if (button_id != UndefinedButton &&
		mouse_pos.x > (position.x - size.x * 0.5) &&
		mouse_pos.x < (position.x + size.x * 0.5) &&
		mouse_pos.x >(position.y - size.y * 0.5) &&
		mouse_pos.x < (position.y + size.y * 0.5)){

		material.color = Vector4::RED;
		if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
			// StageManager::getInstance()->current->onButtonPressed(button_id);
		}
	}
	else {
		material.color = Vector4::WHITE;
	}

	Entity::update(delta_time);
}

void EntityUI::update3D(Vector3 position3d) {
	pos3d = position3d;

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	Stage* stage = StageManager::instance->currStage;

	visible = true;

	Vector3 pos = stage->camera->project(position3d, width, height);
	if (pos.z < 0.0f || pos.z > 1.0f) {
		visible = false;
	}
	else {
		pos.y = height - pos.y;
		position = Vector2(pos.x, pos.y);
	}
}

void EntityUI::addLOD(sMeshLOD mesh_lod) {
	mesh_lods.push_back(mesh_lod);
	std::sort(mesh_lods.begin(), mesh_lods.end(), [](sMeshLOD a, sMeshLOD b) { return a.distance < b.distance; });
}