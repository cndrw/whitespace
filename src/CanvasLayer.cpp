#include <iostream>
#include <fstream>
#include <ranges>
#include <algorithm>
#include <format>
#include <cmath>

#include "raylib.h"
#include "raymath.h"
#include "yaml-cpp/yaml.h"

#include "Application.h"
#include "AssetManager.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "DataPersistanceLayer.h"
#include "SceneElement.h"
#include "Event.h"

auto left_clicked { false };
auto left_down { false };

// temp function should be elswhere...
Rectangle CanvasLayer::transform_to_screen(const Rectangle& rect)
{
    const auto offset = (Vector2) {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2
    };

    const auto res_rect = (Rectangle) {
        .x = (rect.x + m_origin.x - offset.x) * m_scale + offset.x, 
        .y = (rect.y + m_origin.y - offset.y) * m_scale + offset.y, 
        .width = rect.width * m_scale,
        .height = rect.height * m_scale
    };

    return  res_rect;
}

CanvasLayer::CanvasLayer()
{
}  

void CanvasLayer::init()
{
    Core::Application::get()
        .get_layer<UILayer>()
        ->get_asset_explorer()
        ->add_scene_element.add_listener([this] (const auto& asset)
        {
            std::cout << "add: " << asset.path << std::endl;
            add_scene_element(asset);
        });
}

void CanvasLayer::add_scene_element(const Core::Asset& asset)
{
    constexpr auto ppu { 16 };
    auto sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = resolve_naming(asset.path);
    sprite_element->pos = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    sprite_element->texture = asset.texture;
    sprite_element->ppu = ppu;
    sprite_element->width = sprite_element->texture.width * ppu;
    sprite_element->height = sprite_element->texture.height * ppu;
    sprite_element->asset_id = asset.id;
    sprite_element->layer = 0;

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
    const Vector2 wheel_move = GetMouseWheelMoveV();

    if (left_clicked)
    {
        if ((m_focused_sprite_elem = determine_focused_element(cursor_pos)))
        {
            m_sprite_drag_offset = Vector2Subtract(cursor_pos, m_focused_sprite_elem->pos);
        }
    }

    if (!m_focused_sprite_elem)
    {
        on_element_changed.invoke(std::nullopt);

        if (left_down) 
        {
            const Vector2 delta_pos = GetMouseDelta();
            m_origin.x += delta_pos.x;
            m_origin.y += delta_pos.y;
        }
        else if (wheel_move.y != 0)
        {
            m_scale += wheel_move.y * 0.5;
        }

        left_clicked = false;
        left_down = false;
        return;
    }

    if (left_down) 
    {
        Vector2 pos = Vector2Subtract(cursor_pos, m_sprite_drag_offset);
        pos = {
            std::floor(pos.x / m_focused_sprite_elem->ppu) * m_focused_sprite_elem->ppu,
            std::floor(pos.y / m_focused_sprite_elem->ppu) * m_focused_sprite_elem->ppu
        }; 
        
        m_focused_sprite_elem->pos = pos;
        on_element_changed.invoke(*m_focused_sprite_elem);
    }

    if (IsKeyDown(KeyboardKey::KEY_LEFT_CONTROL) && IsKeyPressed(KeyboardKey::KEY_S))
    {
        save_scene();
    }

    left_clicked = false;
    left_down = false;
}

void CanvasLayer::render()
{
    draw_reference_resolution({1920, 1080});

    for (const auto& [_, elements] : m_sprite_elements)
    {
        for (const auto& element : elements)
        {
            // const Vector2 render_pos = { m_origin.x + element->pos.x, m_origin.y + element->pos.y };
            DrawTexturePro(element->texture,
                { 0.0, 0.0, (float)element->texture.width, (float)element->texture.height },
                transform_to_screen(element->rect()), { 0, 0 }, 0, RAYWHITE
            );
        }
    }

    if (m_focused_sprite_elem)
    {
        DrawRectangleLinesEx(
            m_focused_sprite_elem->rect(),
            1,
            MAGENTA
        );
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

void CanvasLayer::save_scene()
{
    YAML::Node scene;

    for (const auto& [_, layer] : m_sprite_elements)
    {
        for (const auto& element : layer)
        {
            scene["SpriteElements"][element->name]["x"] = element->pos.x;
            scene["SpriteElements"][element->name]["y"] = element->pos.y;
            scene["SpriteElements"][element->name]["layer"] = static_cast<int>(element->layer);
            scene["SpriteElements"][element->name]["asset_id"] = element->asset_id;
            std::cout << std::format("Save with id: {}\n", element->asset_id);
        }
    }

    Core::Application::get().get_layer<DataPersitanceLayer>()->save_scene(scene);
}

void CanvasLayer::load_scene(const std::string& scene_name)
{
    auto& app = Core::Application::get();
    const auto scene = app.get_layer<DataPersitanceLayer>()->load_scene(scene_name);

    auto* const am = app.get_asset_manager();
    for (const auto& it : scene["SpriteElements"])
    {
        const std::string name = it.first.as<std::string>();

        constexpr auto ppu { 16 };
        auto sprite_element = std::make_shared<SpriteElement>();
        sprite_element->name = name;
        sprite_element->pos = { (float)it.second["x"].as<int>(), (float)it.second["y"].as<int>() };
        Core::Asset asset = am->get_asset(it.second["asset_id"].as<uint32_t>());
        sprite_element->asset_id = asset.id;
        sprite_element->texture = asset.texture;
        sprite_element->ppu = ppu;
        sprite_element->width = sprite_element->texture.width * ppu;
        sprite_element->height = sprite_element->texture.height * ppu;
        sprite_element->layer = 0;

        m_sprite_elements[0].push_back(sprite_element);
        m_focused_sprite_elem = m_sprite_elements[0].back();

    }
}

void CanvasLayer::draw_reference_resolution(const Vector2 res)
{

    const Rectangle ref_rect = { 0, 0, res.x, res.y };
    DrawRectangleLinesEx(transform_to_screen(ref_rect), 2, ORANGE);
    const std::string res_text = std::format("{}x{}", res.x, res.y);
    constexpr auto padding { 5 };
    constexpr auto font_size { 25 };
    // TODO: why is it no working when i scale it with padding? (only works if applied after)
    Rectangle ref_label = { m_origin.x + padding, m_origin.y - font_size - padding, 0, 0 };
    ref_label = transform_to_screen(ref_label);
    DrawText(
        res_text.c_str(),
        ref_label.x,
        ref_label.y,
        font_size, ORANGE
    );
}

CanvasLayer::~CanvasLayer()
{
}

bool CanvasLayer::on_click()
{
    left_clicked = IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT);
    left_down = IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT);
    return false;
}
