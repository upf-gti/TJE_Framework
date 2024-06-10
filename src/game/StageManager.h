#pragma once
#include <unordered_map>
#include <string>
#include "framework/framework.h"

class Stage;

class StageManager
{
public:
	static StageManager* instance;
	Vector3 ambient_night;
	Vector3 ambient_day;
	bool transitioning;

	Stage* currStage;
	std::unordered_map<std::string, Stage*> stages;

	StageManager();

	void render();
	void update(double dt);
	void changeStage(std::string nextStage);
	void resize(float width, float height);
};

