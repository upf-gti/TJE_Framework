#pragma once

#include "framework/includes.h"
#include "framework/framework.h"
#include <map>

class Stage {

public:

	Stage() {}; 			// Constructor

	virtual void onEnter(Stage* previousStage) {};
	virtual void onLeave(Stage* nextStage) {};

	virtual void onKeyDown(SDL_KeyboardEvent event) {};
	virtual void onMouseWheel(SDL_MouseWheelEvent event) {};
	virtual void onResize(int width, int height) {};
	virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};

	virtual void render() {};
	virtual void update(float seconds_elapsed) {};
};

class StageManager : public Stage {

	static StageManager* instance;

public:

	static StageManager* get_instance() {
		if (instance != nullptr)
			return instance;
		instance = new StageManager();
		return instance;
	}

	Stage* current = nullptr;
	std::map<std::string, Stage*> stages;

	StageManager() {};

	void goTo(const std::string& stageName)
	{
		Stage* nextStage = stages[stageName];
		if (!nextStage)
			return;

		if (current) {
			current->onLeave(nextStage);
		}

		nextStage->onEnter(current);

		// assign new current
		current = nextStage;
	}

	void render();
	void update(float seconds_elapsed) { if (current) current->update(seconds_elapsed); }

	void onKeyDown(SDL_KeyboardEvent event) { if (current) current->onKeyDown(event); }
	void onMouseWheel(SDL_MouseWheelEvent event) { if (current) current->onMouseWheel(event); }
	void onMouseButtonDown(SDL_MouseButtonEvent event) { if (current) current->onMouseButtonDown(event); }
	void onMouseButtonUp(SDL_MouseButtonEvent event) { if (current) current->onMouseButtonUp(event); }
	void onResize(int width, int height) { if (current) current->onResize(width, height); }
};

class PlayStage : public Stage {

	//float win_countdown = 60.0f; // seconds

public:
	PlayStage();

	// Methods overwritten from base class
	void render();
	void update(float delta_time);

	void onEnter(Stage* previousStage);
	void onKeyDown(SDL_KeyboardEvent event);
	void onResize(int width, int height);
};

