#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "UIElements.h"

#include "raylib.h"
#include "raygui.h"

bool UIButton::is_hovered()
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

UIDropDownList::UIDropDownList(
    const Rectangle& rect_p,
    const std::string& header,
    const std::vector<std::string>& items)
    : UIButton(rect_p, [](){}, [](){}), m_header(header)
{
    render = [this]() {
        if (m_state == State::CLOSED)
        {
            GuiLabelButton(rect, m_header.c_str());
        }
        else
        {
            const Rectangle scene_list_rect = {
                rect.x,
                rect.y + rect.height,
                rect.width,
                rect.height * m_items.size()
            };
            DrawRectangleRec(scene_list_rect, LIGHTGRAY); 
            GuiLabelButton(rect, m_header.c_str());
            for (const auto& item : m_items)
            {
                item->render();
            }
        }
    };

    on_click = [this]() {
        if (m_header_hovered)
        {
            m_state = (m_state == State::CLOSED) ? State::OPENED : State::CLOSED;
            return;
        }
        for (const auto& item : m_items)
        {
            if (item->is_hovered())
            {
                item->on_click();
                m_state = State::CLOSED;
                return;
            }
        }

        m_state = State::CLOSED;
    };
}

void UIDropDownList::add_item(const std::string& item, Callback on_click)
{
    m_items.push_back(
        std::make_unique<UIButton>(
            Rectangle {
                rect.x,
                rect.y + rect.height * (m_items.size() + 1),
                rect.width,
                rect.height
            },
            [this, item, pos = m_items.size()]() {
                GuiLabelButton(
                    Rectangle {
                        rect.x,
                        rect.y + rect.height * (pos + 1),
                        rect.width,
                        rect.height
                    },
                    item.c_str()
                );
            },
            on_click
        )
    );
}

bool UIDropDownList::is_hovered()
{
    Vector2 mouse_pos = GetMousePosition();
    m_header_hovered = false;
    if (m_state == State::CLOSED)
    {
        m_header_hovered = CheckCollisionPointRec(mouse_pos, rect);
        return m_header_hovered;
    } 
    else 
    {
        if (CheckCollisionPointRec(mouse_pos, rect))
        {
            m_header_hovered = true;
            return true;
        }

        for (const auto& item : m_items)
        {
            if (CheckCollisionPointRec(mouse_pos, item->rect))
                return true;
        }

        // this is more of a workaround since currently if i press somewhere else
        // on_click() wont get triggered because it is not hovered over...
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
        {
            m_state = State::CLOSED;
        }
        return false;
    }
}
