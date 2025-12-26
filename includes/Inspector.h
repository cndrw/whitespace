#pragma once

#include "raymath.h"

class Inspector
{
public:
    void set_rect(const Rectangle rect);
    void update_content(Vector2 pos); 
    void render();

private:
    void draw_label(const Vector2 pos, const std::string label);

private:
    Rectangle m_rect;
    Vector2 m_inspected_element;
};