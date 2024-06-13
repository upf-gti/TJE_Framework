#pragma once
#include "stage.h"

struct Font {
    Texture* font;
    Vector2 tilesize;
};

struct Text {
    int currchars = 0;
    float currcharupdatetime;
    float offset;
    float starttime;
    float endtime;
    float size;
    std::string content;
    Vector2 position;
    Font* font;
};

struct ColorTransition {
    float start_time;
    float fade_in;
    float duration;
    float fade_out;
    Vector4 color;
};

struct Scene {
    Texture* scene;
    float starttime;
    float endtime;
    Vector2 position;
    Vector2 size;
    Vector2 position_dt;
    Vector2 size_dt;
};

class LoreStageBegin : Stage
{
public:
    LoreStageBegin();

    std::vector<Text> texts;
    std::vector<ColorTransition> transitions;
    std::vector<Scene> scenes;


    void renderPic(Vector2 position, Vector2 size, Texture* diffuse);
    void renderLetter(Texture* font, Vector2 tileSize, char letter, Vector2 position, Vector2 size);
    void renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize);

    void renderSquare(Vector2 barPosition, Vector2 barSize, float percentage, Vector4 color);

    void pushText(std::string content, float offset, float starttime, float endtime, float size, Vector2 position, Font* font);
    void pushTransition(float start_time, float fade_in, float fade_out, float duration, Vector4 color);
    void pushScene(std::string scenepath, float starttime, float endtime, Vector2 pos, Vector2 posdt, Vector2 size, Vector2 sizedt);

    void render(void) override;
    void update(double seconds_elapsed) override;
};