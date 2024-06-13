#include "StageManager.h"
#include "stage.h"
#include "GameStage.h"
#include "IntroStage.h"
#include "loreStageBegin.h"
#include "framework/audio.h"

StageManager* StageManager::instance = NULL;


StageManager::StageManager()
{
	ambient_night = Vector3(0.15, 0.15, 0.25).normalize();
	ambient_day = Vector3(0.15, 0.15, 0.25).normalize();

	stages["IntroStage"] = (Stage*) new IntroStage();
	stages["GameStage"] = (Stage*) new GameStage();
	stages["LoreStageBegin"] = (Stage*) new LoreStageBegin();

	transitioning = false;

	StageManager::instance = this;
	this->currStage = stages["LoreStageBegin"];
}

void StageManager::render() {
	currStage->render();
}

void StageManager::update(double seconds_elapsed) {
	if (transitioning)
	{
		if (currStage->nextStage == "IntroStage") {
			Audio::Play("data/audio/bgm.mp3", 0.7);
		}
		currStage = stages[currStage->nextStage];
		transitioning = false;
	}

	currStage->update(seconds_elapsed);
}