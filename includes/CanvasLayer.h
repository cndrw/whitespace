#pragma once

#include <map>
#include <vector>

#include "raylib.h"

#include "Layer.h"
#include "SceneElement.h"
#include "Event.h"


class CanvasLayer : public Core::Layer
{
public:
    CanvasLayer();
    virtual void init() override;
    virtual void update() override;
    virtual void render() override;
    virtual ~CanvasLayer();
    Core::Event<Vector2> on_element_changed;

private:
    std::shared_ptr<SpriteElement> determine_focused_element(const Vector2 cursor_pos);

private:
    std::map<uint8_t, std::vector<std::shared_ptr<SpriteElement>>> m_sprite_elements;
    std::shared_ptr<SpriteElement> m_focused_sprite_elem;
    Vector2 m_sprite_drag_offset;
};
