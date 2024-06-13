#include "loreStageBegin.h"
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

#include <string>

std::vector<Texture*> scenes;
Font* font1;
Shader* textShader;

void LoreStageBegin::renderPic(Vector2 position, Vector2 size, Texture* diffuse) {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	picshader->enable();

	Mesh quad;
	quad.createQuad(position.x, position.y, size.x, size.y, false);

	Stage* stage = StageManager::instance->currStage;

	picshader->setUniform("u_model", Matrix44());
	picshader->setUniform("u_color", Vector4(1.0f));
	picshader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);

	if (diffuse) {
		picshader->setTexture("u_texture", diffuse, 0);
	}

	quad.render(GL_TRIANGLES);

	picshader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void LoreStageBegin::renderLetter(Texture* font, Vector2 tileSize, char letter, Vector2 position, Vector2 size) {
	Vector2 selectedTile;
	if (letter >= 'a' && letter <= 'z') {
		if (letter > 'n') {
			selectedTile = Vector2(letter - 'a' + 1, 2);
		}
		else {
			selectedTile = Vector2(letter - 'a', 2);
		}	
	}
	else if (letter >= 'A' && letter <= 'Z') {
		if (letter > 'N') {
			selectedTile = Vector2(letter - 'A' + 1, 1);
		}
		else {
			selectedTile = Vector2(letter - 'A', 1);
		}
	}
	else if (letter == -111) {
		std::cout << letter;
		selectedTile = Vector2('N' + 1 - 'A' , 1);
	}
	else if (letter == -79) {
		std::cout << letter;
		selectedTile = Vector2('n' + 1 - 'a' , 2);
	}
	else if (letter == ' ') {
		selectedTile = Vector2(26, 0);
	} else if (letter == '.') selectedTile = Vector2(5, 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	textShader->enable();

	Mesh quad;
	quad.createQuad(position.x, position.y, size.x, size.y, false);

	Stage* stage = StageManager::instance->currStage;

	textShader->setUniform("u_model", Matrix44());
	textShader->setUniform("u_color", Vector4(1.0f));
	textShader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	textShader->setUniform("u_tileSize", tileSize);
	textShader->setUniform("u_tileGrid", Vector2(27, 3));
	textShader->setUniform("u_selectedTile", selectedTile);

	if (font) {
		textShader->setTexture("u_texture", font, 0);
	}

	quad.render(GL_TRIANGLES);

	textShader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void LoreStageBegin::renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize) {
	for (std::string::iterator it = text.begin(); it != text.end(); ++it) {
		renderLetter(font, tilesize, *it, position, tilesize * fontsize);
		position.x += tilesize.x * fontsize;
	}
}

void LoreStageBegin::pushText(std::string content, float offset, float starttime, float size, Vector2 position, Font* font) {
	if (font == nullptr) font = font1;
	Text newtext;
	newtext.content = content;
	newtext.offset = offset;
	newtext.starttime = starttime;
	newtext.size = size;
	newtext.font = font;
	newtext.position = position;

	texts.push_back(newtext);
}

LoreStageBegin::LoreStageBegin()
{
	font1 = new Font();
	font1->font = Texture::Get("data/textures/fontpool.PNG");
	font1->tilesize = Vector2(16, 24);
	textShader = Shader::Get("data/shaders/basic.vs", "data/shaders/text.fs");
	picshader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    // Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	int gamewidth = Game::instance->window_width;
	int gameheight = Game::instance->window_height;

	float nextoffset;
    nextStage = "IntroStage";

	std::string nexttext = "Esta es la historia de nuestro amigo"; float center_x = (gamewidth - 1.5 * nexttext.size() * font1->tilesize.x) / 2 + 5;
	pushText(nexttext, 0.05, 2, 1.5, Vector2(center_x, 500), font1);

	nexttext = "Osuka Reiesu"; center_x = (gamewidth - 1 * nexttext.size() * 3 * font1->tilesize.x) / 2 + 5;
	pushText(nexttext, 0.1, 4.5, 3, Vector2(center_x, 500 - font1->tilesize.y * 2.5), font1);

	//nexttext = "La historia de un pobre desgraciado que inmigro a otro pais"; center_x = (gamewidth - 1 * nexttext.size() * font1->tilesize.x) / 2 + 5;
	//pushText(nexttext, 0.05, 5, 1, Vector2(center_x, 500 - font1->tilesize.y * 1.8), font1);

	//nextoffset = 0.05 * (nexttext.size() + 10);
	//nexttext = "con el objetivo de encontrar una vida mejor.";
	//pushText(nexttext, 0.05, 5 + nextoffset , 1, Vector2(center_x, 500 - font1->tilesize.y * 2.9), font1);


	Audio::Get("data/audio/loredump/type0");
	Audio::Get("data/audio/loredump/type1");
	Audio::Get("data/audio/loredump/type2");
	Audio::Get("data/audio/loredump/type3");
	Audio::Get("data/audio/loredump/type4");
	Audio::Get("data/audio/loredump/type5");
}

void LoreStageBegin::render()
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

	//drawText(Game::instance->window_width / 2.0f, Game::instance->window_height / 2.0f, "AAE", Vector3(((int)Game::instance->time) % 2), 5);

	float time = Game::instance->time;



	for (int i = 0; i < texts.size(); ++i) {
		if (time > texts[i].starttime) {
			if (texts[i].currcharupdatetime + texts[i].offset < time && texts[i].currchars < texts[i].content.size()) {
				texts[i].currchars++;
				texts[i].currcharupdatetime = time;
				std::string currentcontent = texts[i].content;
				if (currentcontent.at(texts[i].currchars - 1) != ' ') {
					std::string audiofile = std::to_string((int)floor(random(5.99))) + ".wav";
					Audio::Play("data/audio/loredump/type" + audiofile);
				}
			}
			renderText(texts[i].font->font, texts[i].content.substr(0, texts[i].currchars), 0, texts[i].position, texts[i].size, texts[i].font->tilesize);
		}
	}

	



	//renderPic(Vector2(100, 50), Vector2(1000, 100), font);

	//std::cout << (int) 'A' << " " << (int)'a' << " " << (int)'ñ';
}

void LoreStageBegin::update(double seconds_elapsed)
{
    if (Input::wasKeyPressed(SDL_SCANCODE_A)) {
        StageManager::instance->transitioning = true;
    }
}