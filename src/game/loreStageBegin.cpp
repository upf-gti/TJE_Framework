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
Texture* gus;
Shader* textShader;

float timemultiplier = 1;
float timeoffset = 0;

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
	else if (letter == ',') selectedTile = Vector2(4, 0);
	else if (letter == ':') selectedTile = Vector2(2, 0);
	else if (letter == '!') selectedTile = Vector2(0, 0);
	else if (letter == '?') selectedTile = Vector2(1, 0);
	else  selectedTile = Vector2(26, 0);

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

void LoreStageBegin::renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize, Vector2 textbox) {
	float origin = position.x;
	std::istringstream wordStream(text);
	std::string word;

	while (wordStream >> word) {
		// Calculate the width of the word
		float wordWidth = word.length() * tilesize.x * fontsize;

		// Check if the word fits in the current line
		if (position.x + wordWidth > textbox.x) {
			// Move to the next line if the word doesn't fit
			position.x = origin;
			position.y -= tilesize.y * fontsize * 1.1;
		}

		// Render each letter of the word
		for (size_t i = 0; i < word.length(); ++i) {
			if (word[i] == static_cast<char>(-79)) {
				// Ignore the previous character if the current character is 'ñ'
				if (i > 0) {
					position.x -= tilesize.x * fontsize;
				}
				renderLetter(font, tilesize, word[i], position, tilesize * fontsize);
			}
			else {
				renderLetter(font, tilesize, word[i], position, tilesize * fontsize);
			}
			position.x += tilesize.x * fontsize;
		}

		// Add space after the word
		position.x += tilesize.x * fontsize;
	}
}

void LoreStageBegin::renderSquare(Vector2 barPosition, Vector2 barSize, float percentage, Vector4 color)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Mesh square;
	if (!squareshader) {
		squareshader = Shader::Get("data/shaders/hud.vs", "data/shaders/square.fs");
	}
	shader = squareshader;

	shader->enable();

	//Creation of the second quad. This one contains the life information. 
	square.createQuad(barPosition.x, barPosition.y, barSize.x, barSize.y, true);
	shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	shader->setUniform("u_color", color);
	shader->setUniform("u_percentage", percentage);

	square.render(GL_TRIANGLES);

	shader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void LoreStageBegin::pushText(std::string content, float offset, float starttime, float endtime, float size, Vector2 position, Vector2 textbox, Font* font, std::string audiopath){
	if (font == nullptr) font = font1;
	Text newtext;
	newtext.content = content;
	newtext.offset = offset;
	newtext.starttime = starttime;
	newtext.endtime = endtime;
	newtext.size = size;
	newtext.font = font;
	newtext.position = position;
	newtext.textboxsize = textbox;

	if (audiopath != "") {
		newtext.hasaudio = true;
		newtext.audiopath = audiopath;
	}

	texts.push_back(newtext);
}

void getCenterX(std::string text, Font* font, float size, float offset, float& centerX, float& textDuration) {
	int gamewidth = Game::instance->window_width;
	int gameheight = Game::instance->window_height;
	int textsize = text.size();
	centerX = (gamewidth - size * textsize * font->tilesize.x) / 2;
	textDuration = textsize * offset;
}

void LoreStageBegin::pushTransition(float start_time, float fade_in, float fade_out, float duration, Vector4 color) {
	ColorTransition transition;
	transition.color = color;
	transition.duration = duration;
	transition.fade_in = fade_in;
	transition.fade_out = fade_out;
	transition.start_time = start_time;

	transitions.push_back(transition);
}

void LoreStageBegin::pushScene(const char* scenepath, float starttime, float endtime, Vector2 pos, Vector2 posdt, Vector2 size, Vector2 sizedt) {
	Scene scene;
	scene.scene = Texture::Get(scenepath);
	scene.starttime = starttime;
	scene.endtime = endtime;
	scene.position = pos;
	scene.position_dt = posdt;
	scene.size = size;
	scene.size_dt = sizedt;

	scenes.push_back(scene);
}

void LoreStageBegin::pushBox(float start_time, float fade_in, float fade_out, float duration, Vector4 color, Vector2 position, Vector2 size) {
	Box box;
	box.color = color;
	box.duration = duration;
	box.fade_in = fade_in;
	box.fade_out = fade_out;
	box.start_time = start_time;
	box.position = position;
	box.size = size;

	boxes.push_back(box);
}

LoreStageBegin::LoreStageBegin()
{
	renderFBO = NULL;

	font1 = new Font();
	font1->font = Texture::Get("data/textures/fontpool.PNG");
	font1->tilesize = Vector2(16, 24);
	textShader = Shader::Get("data/shaders/basic.vs", "data/shaders/text.fs");
	picshader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	squareshader = Shader::Get("data/shaders/hud.vs", "data/shaders/square.fs");

	gus = Texture::Get("data/textures/gus.png");
	// Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	int gamewidth = Game::instance->window_width;
	int gameheight = Game::instance->window_height;

	float textDuration;
	float centerX;
	float size;
	float offset;
	std::string nexttext;
	nextStage = "IntroStage";

	float starttime = 0;
	float duration = 0;

	nexttext = "Esta es la historia de nuestro amigo"; size = 1.5; offset = 0.05; starttime = 2; duration = 5;
	getCenterX(nexttext, font1, size, offset, centerX, textDuration);
	pushText(nexttext, offset, starttime, starttime + duration, size, Vector2(centerX, 500), Vector2(gamewidth), font1);
	starttime += duration;

	nexttext = "Osuka Reiesu"; size = 3; offset = 0.1;
	getCenterX(nexttext, font1, size, offset, centerX, textDuration);
	pushText(nexttext, offset, 4.5, 7, size, Vector2(centerX, 500 - font1->tilesize.y * 2.5), Vector2(gamewidth), font1);

	pushTransition(6, 1, 1, 1, Vector4(1, 1, 1, 1));


	pushScene("data/textures/scene1.png", 7, 120, Vector2(gamewidth /2, gameheight/2 - gameheight/2), Vector2(0, 2), Vector2(gamewidth * 2, gameheight * 2), Vector2(20,20));
	pushBox(9, 1, 1, 110, Vector4(0, 0, 0, 0.7), Vector2(gamewidth/2, gameheight * 0.15 + 50), Vector2(gamewidth * 0.8, gameheight * 0.3));

	offset = 0.045;

	nexttext = "Osuka Reiesu es un joven immigrante que acaba de llegar al barrio de XiwangNan, trabajando bajo un contrato desfavorable en el Bar Leinuozi bajo el mando del malvado Maolixi Keermeneiluo. "; size = 1.2;
	pushText(nexttext, offset, 10, 22, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth*0.9 - 30), font1);

	offset = 0.05;

	pushTransition(21, 1, 1, 0.4, Vector4(1, 1, 1, 1));
	pushScene("data/textures/scene2.png", 22, 66.5, Vector2(gamewidth / 2, gameheight / 2 - gameheight / 3), Vector2(3, 5), Vector2(gamewidth * 2, gameheight * 2), Vector2(-14, -14));

	nexttext = "Maolixi: Osuka! Todavia no has fregado los platos?? Eres un inutil, te doy un trabajo y asi me lo pagas!";
	pushText(nexttext, offset, 23, 33, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "maolixi1.mp3");

	nexttext = "Osuka: Pero señor, ya los he lavado."; 
	pushText(nexttext, offset, 33, 35.2, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "osuka1.mp3");

	nexttext = "Maolixi: Que te calles! Pues los lavas otra vez!!";
	pushText(nexttext, offset, 35, 39, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "maolixi2.mp3");
	nexttext = "Maolixi: Por cierto, recuerda que mañana tienes que venir a jugar al golf! No quiero escuchar ninguna excusa.";
	pushText(nexttext, offset, 39, 46, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "maolixi3.mp3");

	nexttext = "Osuka: Pero señor, mañana tengo que ir a ver el partido de mi hijo!";
	pushText(nexttext, offset, 46, 51, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "osuka2.mp3");

	nexttext = "Maolixi: Pero que mas da, si en el equipo de tu hijo son todos muy malos, ya sabes que van a perder otra vez. Para la proxima buscate una excusa mejor, hombre. Mañana nos vemos en el golf.";
	pushText(nexttext, offset, 51, 66.5, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1, "maolixi4.mp3");

	pushTransition(65.5, 1, 1, 0.4, Vector4(1, 1, 1, 1));

	nexttext = "El joven indignado por todas las ofensas, decide un dia plantar cara a su jefe. Sin embargo, su ansiedad no le permite tomar acciones, asi que decide hacerlo solo en sus sueños, donde es un poderoso mago. "; size = 1.2;
	pushText(nexttext, offset, 67, 80, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1);

	nexttext = "No obstante, Maolixi, al ser la fuente de sus problemas, opone resistencia incluso en los sueños de Osuka, pues su ego no le permite ser difamado ni en los sueños de otros. Podra Osuka derrotar su bloqueo mental para finalmente oponerse a Maolixi en la realidad?"; size = 1.2;
	pushText(nexttext, offset, 80, 110, size, Vector2(gamewidth * 0.1 + 30, gameheight * 0.3 + 20), Vector2(gamewidth * 0.9 - 30), font1);

	pushTransition(94, 7, 1, 100, Vector4(1, 1, 1, 1));
	
	

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
	Audio::Get("data/audio/loredump/maolixi1.mp3");
	Audio::Get("data/audio/loredump/maolixi2.mp3");
	Audio::Get("data/audio/loredump/maolixi3.mp3");
	Audio::Get("data/audio/loredump/maolixi4.mp3");
	Audio::Get("data/audio/loredump/osuka1.mp3");
	Audio::Get("data/audio/loredump/osuka2.mp3");
}

void LoreStageBegin::render()
{

	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera2D->enable();





	//drawText(Game::instance->window_width / 2.0f, Game::instance->window_height / 2.0f, "AAE", Vector3(((int)Game::instance->time) % 2), 5);

	float time = timemultiplier * Game::instance->time + timeoffset;

	int gamewidth = Game::instance->window_width;
	int gameheight = Game::instance->window_height;

	float width = Game::instance->window_width, height = Game::instance->window_height;
	if (!renderFBO) {
		renderFBO = new RenderToTexture();
		renderFBO->create(width, height);
	}
	renderFBO->enable();

	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < scenes.size(); i++) {
		if (time > scenes[i].starttime && time < scenes[i].endtime) {
			renderPic(scenes[i].position, scenes[i].size, scenes[i].scene);
		}
	}
	glDisable(GL_DEPTH_TEST);
	renderFBO->disable();

	Shader* shader = Shader::Get("data/shaders/postfx.vs", "data/shaders/postfx2.fs");
	shader->enable();
	shader->setUniform("iResolution", Vector2(renderFBO->width, renderFBO->height));

	renderFBO->toViewport(shader);

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < boxes.size(); ++i) {
		float start_time = boxes[i].start_time;
		float fadein_time = boxes[i].fade_in;
		float fadeout_time = boxes[i].fade_out;
		float duration = boxes[i].duration;
		float zero_one;
		if (time > start_time && time < start_time + fadein_time + duration) {
			zero_one = clamp((time - start_time) / fadein_time, 0, 1);
			renderSquare(boxes[i].position, boxes[i].size, 1, Vector4(boxes[i].color.x, boxes[i].color.y, boxes[i].color.z, boxes[i].color.w * zero_one));
		}
		else if (time > start_time + fadein_time + duration && time < start_time + fadein_time + duration + fadeout_time) {
			zero_one = clamp((1 - (time - (start_time + fadein_time + duration)) / fadeout_time), 0, 1);
			renderSquare(boxes[i].position, boxes[i].size, 1, Vector4(boxes[i].color.x, boxes[i].color.y, boxes[i].color.z, boxes[i].color.w * zero_one));
		}
	}

	for (int i = 0; i < texts.size(); ++i) {
		if (time > texts[i].starttime && time < texts[i].endtime) {
			if (texts[i].currcharupdatetime + texts[i].offset < time && texts[i].currchars < texts[i].content.size()) {
				texts[i].currchars++;
				texts[i].currcharupdatetime = time;
				std::string currentcontent = texts[i].content;
				if (currentcontent.at(texts[i].currchars - 1) != ' ') {
					if (texts[i].hasaudio && !texts[i].hasplayed) {
						std::cout << "playing audio: " << "data/audio/loredump/" + texts[i].audiopath;
						Audio::Play("data/audio/loredump/" + texts[i].audiopath);
						texts[i].hasplayed = true;
					}
					else if (!texts[i].hasaudio) {
						std::string audiofile = std::to_string((int)floor(random(5.99))) + ".wav";
						Audio::Play("data/audio/loredump/type" + audiofile);
					}
				}
			}
			renderText(texts[i].font->font, texts[i].content.substr(0, texts[i].currchars), 0, texts[i].position, texts[i].size, texts[i].font->tilesize, texts[i].textboxsize);
		}
	}

	
	for (int i = 0; i < transitions.size(); ++i) {
		float start_time = transitions[i].start_time;
		float fadein_time = transitions[i].fade_in;
		float fadeout_time = transitions[i].fade_out;
		float duration = transitions[i].duration;
		float zero_one;
		if (time > start_time && time < start_time + fadein_time + duration) {
			zero_one = clamp((time - start_time) / fadein_time, 0, 1);
			renderSquare(Vector2(gamewidth / 2, gameheight / 2), Vector2(gamewidth, gameheight), 1, Vector4(0, 0, 0, zero_one));
		}
		else if (time > start_time + fadein_time + duration && time < start_time + fadein_time + duration + fadeout_time){
			zero_one = clamp(1 - (time - (start_time + fadein_time + duration)) / fadeout_time, 0, 1);
			renderSquare(Vector2(gamewidth / 2, gameheight / 2), Vector2(gamewidth, gameheight), 1, Vector4(0, 0, 0, zero_one));
			//std::cout << 1 - (time - (start_time + fadein_time + duration)) / fadeout_time << std::endl;
		}
	}
	
	//renderPic(Vector2(200,100), Vector2(100,100), gus);




	//renderPic(Vector2(100, 50), Vector2(1000, 100), font);

	//std::cout << (int) 'A' << " " << (int)'a' << " " << (int)'ñ';
}

void LoreStageBegin::update(double seconds_elapsed)
{

	float time = timemultiplier * Game::instance->time + timeoffset;

    if (Input::wasKeyPressed(SDL_SCANCODE_A) || time > 102) {
        StageManager::instance->transitioning = true;
    }

	seconds_elapsed *= timemultiplier;


	int gamewidth = Game::instance->window_width;
	int gameheight = Game::instance->window_height;

	for (int i = 0; i < scenes.size(); i++) {
		if (time > scenes[i].starttime && time < scenes[i].endtime) {
			scenes[i].size = scenes[i].size + scenes[i].size_dt * seconds_elapsed;
			scenes[i].position = scenes[i].position + scenes[i].position_dt * seconds_elapsed;
		}
	}
}

void LoreStageBegin::resize()
{
	Stage::resize();
	float width = Game::instance->window_width, height = Game::instance->window_height;

	if (!renderFBO) renderFBO = new RenderToTexture();
	renderFBO->create(width, height);
}