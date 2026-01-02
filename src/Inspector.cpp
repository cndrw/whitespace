#include <format>
#include <iostream>

#include "raygui.h"

#include "Inspector.h"
#include "SceneElement.h"
#include "Utils.h"

void Inspector::render()
{
    GuiGroupBox(m_rect, "Inspector");

    if (!m_is_unselected)
    {
        draw_label({10, 10}, std::format("Name: {}", m_focused_element.name));
        draw_label({10, 30}, std::format("X: {}", m_focused_element.pos.x));
        draw_label({10, 50}, std::format("Y: {}", m_focused_element.pos.y));
        draw_label({10, 70}, std::format("Angle: {}", m_focused_element.angle));
        draw_label({10, 90}, std::format("Layer: {}", m_focused_element.layer));
    }
}

void Inspector::draw_label(Vector2 pos, std::string text)
{
    // draw relative to inspector pos
    const auto label_pos = Vector2Add({ m_rect.x, m_rect.y }, pos);
    const Rectangle label_rect =
        { label_pos.x, label_pos.y, (float)m_label_width, (float)m_label_height };
    DRAW_DEBUG_RECTANGLE(label_rect, MAGENTA);
    GuiLabel(label_rect, text.c_str());
}

void Inspector::set_rect(const Rectangle rect)
{
    m_rect = rect;
    m_label_width = rect.width - 20; // 10 padding each side
    m_label_height = 15; 
}

void Inspector::update_content(const std::optional<SpriteElement>& element)
{
    m_is_unselected = !element.has_value();
    if (!m_is_unselected)
    {
        m_focused_element = element.value();
    }

}
