#pragma once

#include "framework/entities/entityMesh.h"
#include "framework/animation.h"
#include "graphics/material.h"

enum eButtonID {
	UndefinedButton,
	PlayButton,
	OptionsButton,
	ExitButton
	// ...
};

class EntityUI : public EntityMesh {

public:
	// Attributes of the derived class  
	bool visible = true;

	eButtonID button_id;

	Vector2 size;
	Vector2 position;

	bool is3d = false;
	Vector3 pos3d;

	float mask = 1.0f;

	EntityUI() { };
	EntityUI(Vector2 pos, Vector2 size, const Material& material, eButtonID button_id = UndefinedButton, const std::string idk_no_cabe_en_la_foto = "") {};
	EntityUI(Vector2 size, const Material& material ) {};
	~EntityUI() {}
	
	Vector3 getPosition() const { return model.getTranslation(); };
	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void update3D(Vector3 position3d);
	void addLOD(sMeshLOD mesh_lod);
};

