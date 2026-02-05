#include <format>
#include <iostream>

#include "raygui.h"

#include "Inspector.h"
#include "SceneElement.h"
// #define EDITOR_DEBUG
#include "Utils.h"

Inspector::Inspector()
{
    m_draw_funcs[(int)DisplayType::SceneElement] = [this] () { draw_scene_element_content(); };
    m_draw_funcs[(int)DisplayType::SpriteElement] = [this] () { draw_sprite_element_content(); };
    m_draw_funcs[(int)DisplayType::AssetElement] = [this] () { draw_asset_element_content(); };
}

void Inspector::render()
{
    static constexpr Color bg_color = { 242, 217, 191, 225 };
    DrawRectangleRec(m_outer_rect, bg_color);
    GuiGroupBox(m_inner_rect, "Inspector");

    if (m_current_disp_t == DisplayType::None) { return; }

    m_draw_funcs[static_cast<int>(m_current_disp_t)]();
}

void Inspector::draw_label(Vector2 pos, std::string text) const
{
    // draw relative to inspector pos
    const auto label_pos = Vector2Add({ m_inner_rect.x, m_inner_rect.y }, pos);
    const Rectangle label_rect =
        { label_pos.x, label_pos.y, (float)m_label_width, (float)m_label_height };
    DRAW_DEBUG_RECTANGLE(label_rect, MAGENTA);
    GuiLabel(label_rect, text.c_str());
}

void Inspector::draw_scene_element_content() const
{
    std::cout << "draw_scene_element_content not yet implemented!\n";
}

void Inspector::draw_sprite_element_content() const
{
    draw_label({10, 10}, std::format("Name: {}", m_focused_sprite.name));
    draw_label({10, 30}, std::format("X: {}", m_focused_sprite.pos.x));
    draw_label({10, 50}, std::format("Y: {}", m_focused_sprite.pos.y));
    draw_label({10, 70}, std::format("Angle: {}", m_focused_sprite.angle));
    draw_label({10, 90}, std::format("Layer: {}", m_focused_sprite.layer));
}

void Inspector::draw_asset_element_content() const
{
    draw_label({10, 10}, std::format("Name: {}", m_focused_asset.path.stem().string().c_str()));
    draw_label({10, 30}, std::format("PPU: {}", m_focused_asset.ppu));

    const float width = m_inner_rect.width - 20.0f;
    const Vec2 pos = { m_inner_rect.x + 10.0f, m_inner_rect.y + m_inner_rect.height - width - 10.0f };
    const Rectangle asset_rect = { pos.x, pos.y, width, width };


    DRAW_DEBUG_RECTANGLE(asset_rect, MAGENTA);
    DrawTexturePro(
        m_focused_asset.texture,
        { 0.0f, 0.0f, (float)m_focused_asset.texture.width, (float)m_focused_asset.texture.height },
        asset_rect,
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

void Inspector::set_rect(const Rectangle rect)
{
    // m_inner_rect = rect;
    m_outer_rect = rect;
    constexpr float padding { 6.0 };
    m_outer_rect = rect;
    m_inner_rect = {
        .x = rect.x + padding,
        .y = rect.y + padding,
        .width = rect.width - 2 * padding,
        .height = rect.height - 2 * padding
    };

    m_label_width = rect.width - 20; // 10 padding each side
    m_label_height = 15; 
}


void Inspector::update_content(const std::optional<SpriteElement>& element)
{
    m_current_disp_t = element.has_value() ? DisplayType::SpriteElement : DisplayType::None;
    if (m_current_disp_t == DisplayType::SpriteElement)
    {
        m_focused_sprite = element.value();
    }
}

void Inspector::update_content(const Core::Asset& element)
{
    m_current_disp_t = DisplayType::AssetElement;
    m_focused_asset = element;
}
