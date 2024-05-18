#include "entityCollider.h"
#include "framework/camera.h"
#include "game/game.h"

#include <algorithm>

EntityCollider::EntityCollider(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic)
	: EntityMesh(mesh, material, name)
{
	this->is_dynamic = is_dynamic;
}
