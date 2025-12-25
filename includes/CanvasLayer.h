#pragma once

#include <map>
#include <vector>

#include "raylib.h"

#include "Layer.h"
#include "SceneElement.h"


class CanvasLayer : public Layer
{
public:
    CanvasLayer();
    virtual void update() override;
    virtual void render() override;
    std::shared_ptr<SpriteElement> determine_focused_element(const Vector2 cursor_pos);
    virtual ~CanvasLayer();

private:
    std::map<uint8_t, std::vector<std::shared_ptr<SpriteElement>>> m_sprite_elements;
    std::shared_ptr<SpriteElement> m_focused_sprite_elem;
    Vector2 m_sprite_drag_offset;
};