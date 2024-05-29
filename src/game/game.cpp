#include "game.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "graphics/material.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/player.h"
#include "StageManager.h"


#include <fstream>
#include <cmath>
#include <unordered_map>
#include <bitset>

Game* Game::instance = NULL;

// Cosas nuevas que he añadido
Game::Game(int window_width, int window_height, SDL_Window* window)
{
	srand(getpid());
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	StageManager::instance = new StageManager();
}


//what to do when the image has to be draw
void Game::render(void)
{
	StageManager::instance->render();
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	StageManager::instance->update(seconds_elapsed);
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	Stage* stage = StageManager::instance->currStage;
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	}
	stage->onKeyDown(event);
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onKeyUp(event);
}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onMouseButtonDown(event);
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onMouseButtonUp(event);
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onMouseWheel(event);
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onGamepadButtonDown(event);
}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
	Stage* stage = StageManager::instance->currStage;

	stage->onGamepadButtonUp(event);
}

void Game::onResize(int width, int height)
{
	Stage* stage = StageManager::instance->currStage;
	std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	stage->camera->aspect = width / (float)height;
	window_width = width;
	window_height = height;
}
