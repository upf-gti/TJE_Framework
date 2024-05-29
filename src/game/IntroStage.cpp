#include "IntroStage.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "graphics/material.h"
#include "framework/entities/enemy.h"
#include "framework/entities/player.h"
#include "framework/entities/entityUI.h"
#include "StageManager.h"

IntroStage::IntroStage()
{
    // Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

    nextStage = "GameStage";
}

void IntroStage::render()
{
    // Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

    drawText(Game::instance->window_width / 2.0f, Game::instance->window_height / 2.0f, "Press the key A to continue!", Vector3(((int) Game::instance->time)%2), 5);
}

void IntroStage::update(double seconds_elapsed)
{
    if (Input::wasKeyPressed(SDL_SCANCODE_A)) {
        StageManager::instance->transitioning = true;
    }
}