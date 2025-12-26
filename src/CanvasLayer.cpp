#include <iostream>
#include <ranges>
#include <algorithm>
#include <format>

#include "CanvasLayer.h"
#include "SceneElement.h"
#include "Event.h"

#include "raylib.h"
#include "raymath.h"

CanvasLayer::CanvasLayer()
{
    const std::filesystem::path assets_dir("D:\\stuff\\repos\\whitespace\\assets");
    std::filesystem::path img = assets_dir / "smiley.png";

    auto sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = "smiley";
    sprite_element->pos = { 200, 200 };
    sprite_element->width = 100;
    sprite_element->height = 100;
    sprite_element->layer = 1;
    sprite_element->texture = LoadTexture(img.string().c_str());

    m_sprite_elements[sprite_element->layer].push_back(sprite_element);

    sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = "heart";
    sprite_element->pos = { 400, 200 };
    sprite_element->width = 100;
    sprite_element->height = 100;
    sprite_element->layer = 1;
    img = assets_dir / "heart.png";
    sprite_element->texture = LoadTexture(img.string().c_str());

    m_sprite_elements[sprite_element->layer].push_back(sprite_element);
}

void CanvasLayer::init()
{
}

void CanvasLayer::update()
{
    const Vector2 cursor_pos = GetMousePosition();

    if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
    {
        if ((m_focused_sprite_elem = determine_focused_element(cursor_pos)))
        {
            m_sprite_drag_offset = Vector2Subtract(cursor_pos, m_focused_sprite_elem->pos);
        }
    }

    if (!m_focused_sprite_elem) return;

    if (IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT)) 
    {
        m_focused_sprite_elem->pos = Vector2Subtract(cursor_pos, m_sprite_drag_offset);
        on_element_changed.invoke(m_focused_sprite_elem->pos);
    }
}

void CanvasLayer::render()
{
    for (const auto& [_, elements] : m_sprite_elements)
    {
        for (const auto& element : elements)
        {
            DrawTexturePro(element->texture,
                { 0.0, 0.0, (float)element->texture.width, (float)element->texture.height },
                element->rect(), { 0, 0 }, 0, RAYWHITE
            );
        }
    }

    if (m_focused_sprite_elem)
    {
        DrawRectangleLinesEx(
            m_focused_sprite_elem->rect(),
            2,
            MAGENTA
        );
    }
}

// TODO: - function rework
//       - maybe split into to seperate functions 
std::shared_ptr<SpriteElement> CanvasLayer::determine_focused_element(const Vector2 cursor_pos)
{
    for (auto& [_, elements] : std::views::reverse(m_sprite_elements))
    {
        for (int i = elements.size() - 1; i >= 0; i--)
        {
            if (CheckCollisionPointRec(cursor_pos, elements[i]->rect()))
            {
                if (elements.size() > 1 && i != static_cast<int>(elements.size() - 1))
                {
                    std::swap(elements.back(), elements[i]);
                }
                return elements.back();
            }
        }
    }
    return nullptr;
}

CanvasLayer::~CanvasLayer()
{
}

