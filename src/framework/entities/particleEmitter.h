#pragma once

#include "framework/entities/EntityMesh.h"
#include "framework/animation.h"
#include "graphics/material.h"


struct sParticle {
	int id;
	Vector3 position;
	float ttl;
	float rotation;
	bool active = false;
};

class ParticleEmitter : EntityMesh {

	int max_particles = 300;
	int active_particles = 0;
	// Particle container
	std::vector<sParticle> particles;

	// Properties of the emitter
	int last_id = 0;
	float emit_rate = 0.1f;
	float emit_timer = 0.f;
	
	Vector3 emit_position = {  };
	Vector3 emit_velocity;
	float random_factor = 0.0f;

	// Properties of the particles
	float max_ttl;
	std::vector<Vector4> colors = { Vector4(1) };
	std::vector<float> sizes;
	std::vector<float> alphas;
	Texture* texture = nullptr;
	int texture_grid_size = 1;
	bool additive_blending = false;
	bool animated_texture = false;
	bool sort_distance = true; // Si queremos ordenar por distancia (z buffer moment)

	bool emission_enabled = true;

	template<typename T>
	T sample(float time, int n, T* data) {
		float time_in_table = time * (n - 1);
		float entry;
		float amount = modf(time_in_table, &entry);
		return data[(int)entry] * (1 - amount) + data[(int)entry + 1] * amount;
	}

	void emit();

public:

	ParticleEmitter();

	void render(Camera* camera);
	void update(float seconds_elapsed);
};


