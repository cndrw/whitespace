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
    using Handle = std::string;
    Handle handle;

    Texture2D texture;
    float width;
    float height;
    float angle;
    int ppu;
    uint8_t layer = 0;

    inline Rectangle rect() const
    {
        return { pos.x, pos.y, width, height };
    }
};


