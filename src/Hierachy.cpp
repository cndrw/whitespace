#include "Hierachy.h"

#include <iostream>
#include <memory>
#include <algorithm>

#include "raygui.h"

static constexpr auto padding_between { 5 };
static constexpr auto padding_left { 5 };
static constexpr auto height { 15 };

Hierachy::Hierachy()
    : UIComponent("Hierachy")
{
}

Hierachy::~Hierachy()
{
    for (auto& e : m_entries)
    {
        delete e;
        e = nullptr;
    }
}

Rectangle Hierachy::get_entry_rect(const uint16_t pos) const
{
    return {
        m_inner_rect.x + padding_left, m_inner_rect.y + 2 + pos * height + padding_between,
        m_inner_rect.width, height
    };
}

void Hierachy::arrange_entries()
{
    for (size_t i = 0; i < m_entries.size(); i++)
    {
        m_entries[i]->rect = get_entry_rect(i);
    }
}

void Hierachy::add_entry(const SpriteElement& elem)
{
    m_entries.push_back(new UIButton(
        get_entry_rect(m_entries.size()),
        [this, id = elem.get_id()] { 
            on_element_selected.invoke(id);
        },
        elem.name
    ));
}

void Hierachy::remove_entry(const SpriteElement& elem)
{
   m_entries.erase(std::ranges::find_if(m_entries, [&elem] (auto& e) { return elem.name == e->text; }));
   arrange_entries();
}

void Hierachy::update_name(const std::string_view before, const std::string_view after)
{
    for (auto& entry : m_entries)
    {
        if (entry->text == before)
        {
            entry->text = after;
            return;
        }
    }
}

bool Hierachy::process_input()
{
    for (const auto& entry : m_entries)
    {
        if (entry->is_hovered())
        {
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                entry->on_click();
                return true;
            }
        }
    }

    return CheckCollisionPointRec(GetMousePosition(), m_outer_rect);
}

void Hierachy::render_impl()
{
    for (const auto& e : m_entries)
    {
        e->render();
    }
}

