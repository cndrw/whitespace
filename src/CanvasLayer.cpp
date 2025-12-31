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

    try
    {
        load_scene();
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << std::format("{} -> element not found in map - unkown asset id?\n", e.what());
    }
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

void CanvasLayer::load_scene()
{
    // YAML::Node project_file = YAML::LoadFile(path.string());

    // // TODO: project file read logic should be elsewhere...
    // std::filesystem::path asset_root = path.parent_path() / project_file["assetRoot"].as<std::string>();
    // std::filesystem::path scenes_root = path.parent_path() / project_file["scenesRoot"].as<std::string>();

    // const auto scene = YAML::LoadFile((scenes_root / "scene.yaml").string());
    auto& app = Core::Application::get();
    const auto scene = app.get_layer<DataPersitanceLayer>()->load_scene("scene.yaml");

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

CanvasLayer::~CanvasLayer()
{
}
