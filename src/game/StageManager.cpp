#include "StageManager.h"
#include "stage.h"
#include "GameStage.h"

StageManager* StageManager::instance = NULL;
GameStage* secondStage;

StageManager::StageManager()
{
	secondStage = new GameStage();

	StageManager::instance = this;
	this->currStage = (Stage*) secondStage;
}

void StageManager::render() {
	currStage->render();
}

void StageManager::update(float seconds_elapsed) {
	currStage->update(seconds_elapsed);
}