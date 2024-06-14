#pragma once

#include "framework/entities/entity_mesh.h"
#include "framework/animation.h"
#include "framework/extra/pathfinder/PathFinder.h"
#include "framework/extra/pathfinder/Dijkstra.h"
#include "graphics/material.h"
#include "game/world.h"
#include "framework/input.h"
#include "game/entities/entity_player.h"
#include "framework/entities/entity_gun.h"
#include "framework/input.h"

EntityGun::EntityGun() {

}

EntityGun::EntityGun(Mesh* mesh, const std::string& name = "") {
	
	this->mesh = mesh;
	this->name = name;

	Material blaster_mat;
	blaster_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	//blaster_mat.diffuse = new Texture();
	//blaster_mat.diffuse->load("data/meshes/blaster/blasterG.mtl");

	this->material = blaster_mat;
}

void EntityGun::fire(Vector3 origin, Vector3 direction)
{
	if (ammo > 0) {
		//render projectil
		 if (Input::wasButtonPressed(SDL_BUTTON_LEFT)) {
			 ammo -= 1;
		 }
		 else if (Input::wasButtonPressed(SDL_BUTTON_RIGHT)) {
			 ammo -= 5;
		 }
	}	
}

