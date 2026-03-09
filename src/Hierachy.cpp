#include "Hierachy.h"

#include <iostream>
#include <memory>
#include <algorithm>

#include "raygui.h"


Hierachy::Hierachy()
    : UIComponent("Hierachy")
{
}

void Hierachy::add_entry(const SpriteElement& elem)
{
    // TODO: rect should be said when initializing... but current impl with set_rect is difficult
    m_scroll_view.rect = m_inner_rect;
    m_scroll_view.add_entry(elem, [this, id = elem.get_id()] { on_element_selected.invoke(id); });
}

void Hierachy::remove_entry(const SpriteElement& elem)
{
    m_scroll_view.remove_entry(elem);
}

void Hierachy::update_name(const std::string_view before, const std::string_view after)
{
    m_scroll_view.update_entry(before, after);
}

bool Hierachy::process_input()
{
    return m_scroll_view.process_input() || CheckCollisionPointRec(GetMousePosition(), m_outer_rect);
}

void Hierachy::render_impl()
{
    m_scroll_view.render();
}

