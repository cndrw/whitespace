#pragma once

#include <optional>
#include <functional>

#include "raymath.h"

#include "SceneElement.h"
#include "AssetManager.h"

enum class DisplayType
{
    None = -1,
    SceneElement, SpriteElement, AssetElement
};

class Inspector
{
public:
    Inspector();
    void set_rect(const Rectangle rect);
    void update_content(const std::optional<SpriteElement>& element); 
    void update_content(const Core::Asset& element); 
    void render();
    Rectangle get_rect() const { return m_rect; }

private:
    void draw_label(const Vector2 pos, const std::string label) const;
    void draw_scene_element_content() const;
    void draw_sprite_element_content() const;
    void draw_asset_element_content() const;

private:
    Rectangle m_rect;
    int m_label_width, m_label_height;
    std::array<std::function<void()>, 3> m_draw_funcs;
    DisplayType m_current_disp_t = DisplayType::None;
    SpriteElement m_focused_sprite;
    Core::Asset m_focused_asset;
};