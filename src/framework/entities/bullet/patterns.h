#pragma once
#ifndef PATTENRS_H
#define PATTERNS_H

#include "framework/entities/bullet/bulletAuto.h"
#include "framework/entities/bullet/bulletNormal.h"
#include "framework/entities/bullet/bulletSniper.h"


static class Patterns {
public:
	static void autoAim(Vector3 objective, Vector3 direction, Matrix44 model, std::vector<Bullet*>& bullets, int amount, Shader* shader, Texture* texture, Mesh* mesh) {
		Material mat = Material();
		model.translate(Vector3(0,1,0));
		mat.diffuse = texture;
		mat.shader = shader;
		Bullet* b = new BulletAuto(mesh, mat, Vector3(0, 700, 0), direction, model);
		bullets.push_back(b);
	}

	static void circle(Vector3 objective, Vector3 direction, Matrix44 model, std::vector<Bullet*>& bullets, int amount, Shader* shader, Texture* texture, Mesh* mesh) {
		Material mat = Material();

		mat.diffuse = texture;
		mat.shader = shader;
		for (int i = 0; i < amount; i++) {
			Bullet* b = new BulletNormal(mesh, mat, direction, model);
			b->model.rotate(2 * PI * i / amount, Vector3(0, 1, 0));
			bullets.push_back(b);
		}
	}

	static void shotgun(Vector3 objective, Vector3 direction, Matrix44 model, std::vector<Bullet*>& bullets, int amount, Shader* shader, Texture* texture, Mesh* mesh) {
		Material mat = Material();
		mat.diffuse = texture;
		mat.shader = shader;
		for (int i = 0; i < amount; i++) {
			Bullet* b = new BulletNormal(mesh, mat, direction, model, BULLET_SPD + (BULLET_SPD/10) * i);
			b->model.rotate(random(-PI / 8, PI / 8), Vector3(0, 1, 0));
			b->model.rotate(random(-PI / 8, 0), Vector3(1, 0, 0));
			bullets.push_back(b);
		}
	}

	static void sniper(Vector3 objective, Vector3 direction, Matrix44 model, std::vector<Bullet*>& bullets, int amount, Shader* shader, Texture* texture, Mesh* mesh) {
		Material mat = Material();

		mat.diffuse = texture;
		mat.shader = shader;
		Bullet* b = new BulletSniper(mesh, mat, direction, model, 0.001);
		b->acceleration = 0;
		b->opacity_dec = 1;
		bullets.push_back(b);
	}

};

#endif