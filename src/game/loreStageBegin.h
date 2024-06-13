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
    float size;
    std::string content;
    Vector2 position;
    Font* font;
};

class LoreStageBegin : Stage
{
public:
    LoreStageBegin();

    std::vector<Text> texts;


    void renderPic(Vector2 position, Vector2 size, Texture* diffuse);
    void renderLetter(Texture* font, Vector2 tileSize, char letter, Vector2 position, Vector2 size);
    void renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize);

    void renderSquare(Vector2 barPosition, Vector2 barSize, float percentage, Vector4 color);

    void pushText(std::string content, float offset, float starttime, float size, Vector2 position, Font* font);
    void render(void) override;
    void update(double seconds_elapsed) override;
};