#include "StageManager.h"
#include "stage.h"
#include "GameStage.h"
#include "IntroStage.h"

StageManager* StageManager::instance = NULL;


StageManager::StageManager()
{
	ambient_night = Vector3(0.15, 0.15, 0.25).normalize();
	ambient_day = Vector3(0.15, 0.15, 0.25).normalize();

	stages["IntroStage"] = (Stage*) new IntroStage();
	stages["GameStage"] = (Stage*) new GameStage();

	transitioning = false;

	StageManager::instance = this;
	this->currStage = stages["IntroStage"];
}

void StageManager::render() {
	currStage->render();
}

void StageManager::update(double seconds_elapsed) {
	if (transitioning)
	{
		currStage = stages[currStage->nextStage];
		transitioning = false;
	}

	currStage->update(seconds_elapsed);
}