#include "StageManager.h"
#include "stage.h"

StageManager* StageManager::instance = NULL;
Stage* secondStage;

StageManager::StageManager()
{
	secondStage = new Stage();

	StageManager::instance = this;
	this->currStage = (Stage*) secondStage;
}

void StageManager::render() {
	currStage->render();
}

void StageManager::update(float seconds_elapsed) {
	currStage->update(seconds_elapsed);
}