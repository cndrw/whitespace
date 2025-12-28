#include <iostream>
#include <ranges>
#include <algorithm>
#include <format>

#include "raylib.h"
#include "raymath.h"

#include "Application.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "SceneElement.h"
#include "Event.h"


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
    Core::Application::get()
        .get_layer<UILayer>()
        ->get_asset_explorer()
        ->add_scene_element.add_listener([this] (const auto& path)
        {
            add_scene_element(path);
        });
}

void CanvasLayer::add_scene_element(const std::filesystem::path& path)
{
    auto sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = resolve_naming(path);
    sprite_element->pos = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    sprite_element->width = 100;
    sprite_element->height = 100;
    sprite_element->layer = 0;
    sprite_element->texture = LoadTexture(path.string().c_str());

    m_sprite_elements[0].push_back(sprite_element);
    m_focused_sprite_elem = m_sprite_elements[0].back();
}

std::string CanvasLayer::resolve_naming(const std::filesystem::path& path) const
{
    const auto& name = path.stem().string();
    
    std::vector<std::string> matches;
    for (const auto& [_, layer] : m_sprite_elements)
    {
        for (const auto& element : layer)
        {
            if (element->name == name)
            {
                matches.push_back(element->name);
                continue;
            }

            if (const auto idx = element->name.find('_'); idx != std::string::npos)
            {
                if (element->name.substr(0, idx) == name)
                {
                    matches.push_back(element->name);
                }
            }
        }
    }

    int max_value = 0;
    for (const auto& element : matches)
    {
        // TODO: currently something like "heart_2kdfjg" is treated as "heart_2"
        const char* token = "";
        if (const auto idx = element.find('_'); idx != std::string::npos)
        {
            token = element.substr(idx + 1, element.length()).c_str();
            max_value = std::max(max_value, std::atoi(token));
        }
    }

    return std::format("{}_{}", name, max_value + 1);
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

    if (!m_focused_sprite_elem)
    {
        on_element_changed.invoke(std::nullopt);
        return;
    }

    if (IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT)) 
    {
        m_focused_sprite_elem->pos = Vector2Subtract(cursor_pos, m_sprite_drag_offset);
        on_element_changed.invoke(*m_focused_sprite_elem);
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

        draw_arrow(40, 8, 0, RED);
        draw_arrow(40, 8, -90, BLUE);
    }
}

// TODO: - function rework
//       - maybe split into to seperate functions 
std::shared_ptr<SpriteElement> CanvasLayer::determine_focused_element(const Vector2 cursor_pos)
{
    for (auto& [_, elements] : m_sprite_elements | std::views::reverse)
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

void CanvasLayer::draw_arrow(
    const int width, const int height, const float angle, const Color color) const
{
    const auto center = (Vector2) {
        m_focused_sprite_elem->pos.x + m_focused_sprite_elem->width / 2,
        m_focused_sprite_elem->pos.y + m_focused_sprite_elem->height / 2
    };

    DrawRectanglePro(
        (Rectangle) { center.x, center.y, (float)width, (float)height },
        (Vector2) { -(float)height / 2, (float)height / 2 },
        angle,
        color 
    );

    constexpr auto r { 8 };
    Vector2 arrow_tip[] = 
    {
        Vector2Add(Vector2Rotate({ (float)width, r }, angle * DEG2RAD), center),
        Vector2Add(Vector2Rotate({ (float)width + r + 10, 0 }, angle * DEG2RAD), center),
        Vector2Add(Vector2Rotate({ (float)width, -r }, angle * DEG2RAD), center)
    };

    DrawTriangle(arrow_tip[0], arrow_tip[1], arrow_tip[2], color);
}


CanvasLayer::~CanvasLayer()
{
}

