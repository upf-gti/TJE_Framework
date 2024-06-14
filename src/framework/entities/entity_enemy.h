#pragma once

#include "framework/entities/entity_mesh.h"
#include "framework/animation.h"
#include "framework/extra/pathfinder/PathFinder.h"
#include "framework/extra/pathfinder/Dijkstra.h"
#include "graphics/material.h"
#include "game/world.h"
#include "framework/input.h"
#include "game/entities/entity_player.h"


struct WayPoint : public DijkstraNode
{
	WayPoint() {}
	WayPoint(const Vector3& position) : position(position){ }

	void addLink(WayPoint* waypoint, float distance) {
		addChild(waypoint, distance);
		waypoint->addChild(this, distance);
	}

	Vector3 position;
	
};

class EntityEnemy : public EntityCollider {

	enum eActions {SPAWN, CHASE, ATTACK, DIE, DANCE};

	float fov = 0.0f;

	//Waypoints
	bool walk_forwards = true;
	int waypoint_index = 0;
	std::vector<WayPoint> path;
	float mod = 1.0;
	float ar = 10.0;
	float healthbar;
	float damage = 15.0 * mod;

	std::vector<EntityEnemy> enemies;

	eActions state;

public:
	EntityEnemy();
	EntityEnemy(Mesh* mesh, const std::string& name = "");
	~EntityEnemy() {};

	// Vel in move the enemy
	Vector3 velocity;

	float walk_speed = 0.2f;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float seconds_elapsed);

	//Methods from class
	void dif_mod(int difficulty);
	void chase(float seconds_elapsed);
	void attack(EntityPlayer player);
	void die(EntityEnemy enemy);
	bool isdead(EntityEnemy enemy);
	void dance(EntityEnemy enemy);
	void choosedrop();
	void spawn_drop();
	void lookAtTarget(Vector3 target, float seconds_elapsed);
	void followPath(float seconds_elpased);
};