#pragma once
#include "stage.h"

class IntroStage : Stage
{
public:
    IntroStage();

    void render(void) override;
    void update(double seconds_elapsed) override;
};