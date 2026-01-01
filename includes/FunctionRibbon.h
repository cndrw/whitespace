#pragma once

#include "raylib.h"

class FunctionRibbon
{
public:
    FunctionRibbon() = default;
    void render();
    void set_rect(Rectangle rect);

private:
    Rectangle m_rect;
};