#pragma once

#include "entity_bullet.h"
#include "graphics/material.h"

EntityBullet::EntityBullet(Mesh* mesh, Material& mat, const std::string& name) {
	
	this->mesh = mesh;
	this->material = mat;
	this->name = name;

}

void EntityBullet::render(Camera* camera)
{
}

void EntityBullet::update(float seconds_elapsed)
{
}

void EntityBullet::ithits()
{
	//EntityCollider::getCollisions();
}
