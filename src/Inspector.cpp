#include <format>

#include "raygui.h"

#include "Inspector.h"

void Inspector::render()
{
    GuiGroupBox(m_rect, "Inspector");

    draw_label({10, 30}, std::format("X: {}", m_inspected_element.x));
    draw_label({10, 50}, std::format("Y: {}", m_inspected_element.y));
}

void Inspector::draw_label(const Vector2 pos, const std::string label)
{
    // draw relative to inspector pos
    const auto label_pos = Vector2Add({ m_rect.x, m_rect.y }, pos);
    GuiLabel({ label_pos.x, label_pos.y, 50, 10}, label.c_str());
}

void Inspector::set_rect(const Rectangle rect)
{
    m_rect = rect;
}

void Inspector::update_content(Vector2 pos)
{
    m_inspected_element = pos;
}
