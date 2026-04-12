#pragma once

#include <filesystem>
#include <string>

#include "raylib.h"

#include "Utils.h"

class SceneElement
{
public:
    std::string name;
    Vec2 pos;
};


class SpriteElement : public SceneElement
{
public:
    SpriteElement()
        : m_id{id_counter++} { }

    using Handle = std::string;
    Handle handle;

    float width;
    float height;
    float angle;
    uint8_t layer = 0;

private:
    inline static uint16_t id_counter = 0;
    uint16_t m_id;

public:
    uint16_t get_id() const { return m_id; }
    inline Rectangle rect() const
    {
        return { pos.x, pos.y, width, height };
    }
};


