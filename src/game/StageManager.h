#pragma once
#include <unordered_map>
#include <string>
class Stage;

class StageManager
{
public:
	static StageManager* instance;

	bool transitioning;

	Stage* currStage;
	std::unordered_map<std::string, Stage*> stages;

	StageManager();

	void render();
	void update(double dt);
	void changeStage(std::string nextStage);
	void resize(float width, float height);
};

