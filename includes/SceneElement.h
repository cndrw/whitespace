#pragma once

#include <filesystem>
#include <string>

#include "raylib.h"

class SceneElement
{
public:
    std::string name;
    Vector2 pos;
};


class SpriteElement : public SceneElement
{
public:
    float width;
    float height;
    float angle;
    Texture2D texture;
    uint8_t layer = 0;

    inline Rectangle rect() const
    {
        return { pos.x, pos.y, width, height };
    }
};


