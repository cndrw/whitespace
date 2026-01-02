#pragma once

#include <optional>

#include "raymath.h"

#include "SceneElement.h"

class Inspector
{
public:
    void set_rect(const Rectangle rect);
    void update_content(const std::optional<SpriteElement>& element); 
    void render();

private:
    void draw_label(const Vector2 pos, const std::string label);

private:
    Rectangle m_rect;
    int m_label_width, m_label_height;
    SpriteElement m_focused_element;
    bool m_is_unselected;
};