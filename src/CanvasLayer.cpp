#include <iostream>
#include <ranges>
#include <algorithm>

#include "CanvasLayer.h"
#include "SceneElement.h"

#include "raylib.h"
#include "raymath.h"

CanvasLayer::CanvasLayer()
{
    const std::filesystem::path assets_dir("D:\\stuff\\repos\\whitespace\\assets");
    const std::filesystem::path img = assets_dir / "Homescreen.png";

    auto sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = "test_sprite";
    sprite_element->pos = { 200, 200 };
    sprite_element->width = 100;
    sprite_element->height = 100;
    sprite_element->layer = 1;
    sprite_element->texture = LoadTexture(img.string().c_str());

    m_sprite_elements[sprite_element->layer].push_back(sprite_element);

    sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = "test_sprite2";
    sprite_element->pos = { 400, 200 };
    sprite_element->width = 100;
    sprite_element->height = 100;
    sprite_element->layer = 1;
    sprite_element->texture = LoadTexture(img.string().c_str());

    m_sprite_elements[sprite_element->layer].push_back(sprite_element);
};

Vector2 sprite_drag_offset;

void CanvasLayer::update()
{
    if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
    {
        m_focused_sprite_elem = determine_focused_element();
    }

    bool found = false;
    for (auto& [_, elements] : std::views::reverse(m_sprite_elements))
    {
        for (auto& element : elements)
        {
            if (found) continue;
            const Vector2 cursor_pos = GetMousePosition();

            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(cursor_pos, element->rect()))
            {
                sprite_drag_offset = Vector2Subtract(cursor_pos, element->pos);
                element->is_focused = true;
                found = true;
            }

            if (IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT) && element->is_focused)
            {
                const Vector2 new_pos = Vector2Subtract(cursor_pos, sprite_drag_offset);
                element->pos.x = new_pos.x;
                element->pos.y = new_pos.y;
            }
            else 
            {
                element->is_focused = false;
            }
        }
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

std::shared_ptr<SpriteElement> CanvasLayer::determine_focused_element()
{
    const Vector2 cursor_pos = GetMousePosition();
    for (auto& [_, elements] : std::views::reverse(m_sprite_elements))
    {
        for (size_t i = 0; i < elements.size(); i++)
        {
            if (CheckCollisionPointRec(cursor_pos, elements[i]->rect()))
            {
                if (elements.size() > 1 && i != 0)
                {
                    std::swap(elements[0], elements[i]);
                }
                return elements[i];
            }
        }
    }
    return nullptr;
}

void CanvasLayer::put_front_in_layer(std::shared_ptr<SpriteElement> sprite_element)
{
    const auto layer = sprite_element->layer;
    if (m_sprite_elements[layer].size() > 1 && m_sprite_elements[layer][0] != sprite_element)
    {
        printf("swap: %p\n", &sprite_element);
        std::swap(sprite_element, m_sprite_elements[layer][0]);
    }
}

CanvasLayer::~CanvasLayer()
{
}

