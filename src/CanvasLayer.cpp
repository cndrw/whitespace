#include <iostream>
#include <fstream>
#include <ranges>
#include <algorithm>
#include <format>
#include <cmath>

#include "raylib.h"
#include "raymath.h"

#include "Application.h"
#include "AssetManager.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "DataPersistanceLayer.h"
#include "SceneElement.h"
#include "Event.h"
#include "Utils.h"

static constexpr auto SCROLL_SPEED { 0.05f };


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

    return res_rect;
}

Vec2 CanvasLayer::transform_to_screen(const Vec2& vec)
{
    const auto offset = (Vector2) {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2
    };

    return {
        (vec.x + m_origin.x - offset.x) * m_scale + offset.x, 
        (vec.y + m_origin.y - offset.y) * m_scale + offset.y
    };
}

Vec2 CanvasLayer::transform_to_canvas(const Vec2& vec)
{
    const auto offset = (Vector2) {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2
    };

   return {
        ((vec.x - offset.x)  / m_scale) - m_origin.x + offset.x,
        ((vec.y - offset.y)  / m_scale) - m_origin.y + offset.y
    };
}

CanvasLayer::CanvasLayer()
{
}  

void CanvasLayer::init()
{
    auto* ui_layer = Core::Application::get().get_layer<UILayer>();

    ui_layer->get_component<AssetExplorer>()
    ->add_scene_element.add_listener([this] (const auto& asset)
    {
        add_scene_element(asset);
    });


    ui_layer->get_component<Inspector>()
    ->on_sprite_elem_changed.add_listener([this] (SpriteElement changed_elem)
    {
        std::shared_ptr<SpriteElement> ref;
        size_t index = 0; 
        uint8_t layer_before = 0;

        for (auto& layer : m_sprite_elements | std::views::values)
        {
            index = 0;
            for (auto& elem : layer)
            {
                if (elem->get_id() == changed_elem.get_id())
                {
                    if (elem->name != changed_elem.name)
                    {
                        on_element_name_changed.invoke(elem->name, changed_elem.name);
                    }
                    layer_before = elem->layer;
                    *elem = changed_elem;
                    ref = elem;
                    break;
                }
                index++;
            }
        } 

        // TODO (maybe): if a layer has now elements (after it had some) -> it is not deleted from the map
        // put sprite element in new layer
        m_sprite_elements[ref->layer].push_back(ref);
        auto& layer = m_sprite_elements[layer_before];
        layer.erase(layer.begin() + index);
    });
}

void CanvasLayer::add_scene_element(const Core::Asset& asset)
{
    constexpr auto ppu { 16 };
    const Vec2 mpos = GetMousePosition();
    auto sprite_element = std::make_shared<SpriteElement>();
    sprite_element->name = resolve_naming(asset.rel_path);
    // sprite_element->texture = asset.texture;
    sprite_element->pos = mpos;
    sprite_element->ppu = ppu;
    sprite_element->width = asset.texture.width * ppu;
    sprite_element->height = asset.texture.height * ppu;
    sprite_element->handle = asset.name;
    sprite_element->layer = 0;

    m_sprite_elements[0].push_back(sprite_element);
    m_focused_sprite_elem = m_sprite_elements[0].back();
    m_sprite_drag_offset = { m_focused_sprite_elem->width / 2, m_focused_sprite_elem->height / 2 };

    on_element_added.invoke(*sprite_element);
}

void CanvasLayer::remove_scene_element(const std::shared_ptr<SpriteElement>& element)
{
    auto& elements = m_sprite_elements[element->layer];
    elements.erase(
        std::remove_if(
            elements.begin(),
            elements.end(),
            [this](const auto& elem) { return elem == m_focused_sprite_elem; }
        ),
        elements.end()
    );
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
}

void CanvasLayer::render()
{
    draw_reference_resolution({1920, 1080});
    const auto* am = Core::Application::get().get_asset_manager();

    for (const auto& [_, elements] : m_sprite_elements)
    {
        for (const auto& element : elements)
        {
            const auto texture = am->get_asset(element->handle).texture;

            Vec2 midpoint = element->pos + Vec2{ element->width / 2, element->height / 2 };
            Vec2 offset =  element->pos - midpoint;

            const float angle = element->angle * DEG2RAD; 
            Vec2 rotated = Vec2{
                offset.x * cos(angle) - offset.y * sin(angle),
                offset.x * sin(angle) + offset.y * cos(angle)
            };

            Vec2 pos = element->pos + (rotated - offset);

            DrawTexturePro(texture,
                { 0.0, 0.0, (float)texture.width, (float)texture.height },
                transform_to_screen({ pos.x, pos.y, element->width, element->height }), { 0, 0 }, element->angle, RAYWHITE
            );

#ifdef EDITOR_DEBUG
            DrawRectangleLinesEx(
                transform_to_screen(element->rect()),
                1,
                MAGENTA
            );
            
            DrawLine(midpoint.x, midpoint.y, midpoint.x + offset.x, midpoint.y + offset.y, GREEN);
            DrawLine(midpoint.x, midpoint.y, midpoint.x + rotated.x, midpoint.y + rotated.y, BLUE);
#endif 
        }
    }

    if (m_focused_sprite_elem)
    {
        DrawRectangleLinesEx(
            transform_to_screen(m_focused_sprite_elem->rect()),
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
            if (CheckCollisionPointRec(cursor_pos, transform_to_screen(elements[i]->rect())))
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
    auto& app = Core::Application::get();

    const auto* am = app.get_asset_manager();
    scene["project"] = app.get_layer<AppLayer>()->get_cur_project_name();
    scene["editor"]["zoom"] = m_scale;
    scene["editor"]["offset"]["x"] = m_origin.x;
    scene["editor"]["offset"]["y"] = m_origin.y;

    for (const auto& [_, layer] : m_sprite_elements)
    {
        for (const auto& element : layer)
        {
            scene["SpriteElements"][element->name]["x"] = element->pos.x;
            scene["SpriteElements"][element->name]["y"] = element->pos.y;
            scene["SpriteElements"][element->name]["angle"] = element->angle;
            scene["SpriteElements"][element->name]["layer"] = static_cast<int>(element->layer);
            scene["SpriteElements"][element->name]["asset_ref"] = am->get_asset(element->handle).rel_path.string();
            std::cout << std::format("Save with asset_ref: {}\n", element->handle);
        }
    }

    app.get_layer<DataPersitanceLayer>()->save_scene(scene);
}

void CanvasLayer::clear_scene()
{
    // clear current canvas
    m_sprite_elements.clear();
    m_focused_sprite_elem = nullptr;
}

void CanvasLayer::load_scene(const YAML::Node& scene)
{
    clear_scene();

    auto& app = Core::Application::get();

    m_scale = scene["editor"]["zoom"].as<float>();
    m_origin.x = scene["editor"]["offset"]["x"].as<float>();
    m_origin.y = scene["editor"]["offset"]["y"].as<float>();

    auto* const am = app.get_asset_manager();
    for (const auto& it : scene["SpriteElements"])
    {
        const std::string name = it.first.as<std::string>();

        constexpr auto ppu { 16 };
        auto sprite_element = std::make_shared<SpriteElement>();
        sprite_element->name = name;
        sprite_element->pos = { it.second["x"].as<float>(), it.second["y"].as<float>() };
        sprite_element->angle = it.second["angle"].as<float>();

        auto asset_path = std::filesystem::path(it.second["asset_ref"].as<std::string>());
        Handle asset_handle = asset_path.stem().string();
        Core::Asset asset = !am->exists(asset_handle) ?
                            am->add_asset_from_relative(asset_path) :
                            am->get_asset(asset_handle);

        sprite_element->handle = asset.name; 
        // sprite_element->texture = asset.texture;
        sprite_element->ppu = ppu; // asset.ppu;
        sprite_element->width = asset.texture.width * ppu;
        sprite_element->height = asset.texture.height * ppu;
        sprite_element->layer = 0;

        m_sprite_elements[0].push_back(sprite_element);
        on_element_added.invoke(*sprite_element);
    }
}

void CanvasLayer::focus_element(const uint16_t id)
{
    for (const auto& layers : m_sprite_elements | std::views::values)
    {
        for (const auto& element : layers)
        {
            if (element->get_id() == id)
            {
                m_focused_sprite_elem = element;
                on_element_changed.invoke(*m_focused_sprite_elem);
                return;
            }
        }
    }
}

void CanvasLayer::draw_reference_resolution(const Vector2 res)
{

    const Rectangle ref_rect = { 0, 0, res.x, res.y };
    DrawRectangleLinesEx(transform_to_screen(ref_rect), 2, ORANGE);
    const std::string res_text = std::format("{}x{}", res.x, res.y);
    constexpr auto padding { 3 };
    constexpr auto font_size { 25 };
    // TODO: why is it no working when i scale it with padding? (only works if applied after)
    Rectangle ref_label = { padding, -(font_size + padding)  / m_scale , 0, 0 };
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

bool CanvasLayer::process_input()
{
    const Vec2 cursor_pos = GetMousePosition();
    const Vec2 wheel_move = GetMouseWheelMoveV();

    if (IsKeyDown(KeyboardKey::KEY_LEFT_CONTROL) && IsKeyPressed(KeyboardKey::KEY_S))
    {
        save_scene();
        return true;
    }

    if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
    {
        if ((m_focused_sprite_elem = determine_focused_element(cursor_pos)))
        {
            m_sprite_drag_offset = cursor_pos - transform_to_screen(m_focused_sprite_elem->pos);
        }
    }

    if (wheel_move.y != 0)
    {
        const float delta = m_scale  * wheel_move.y * SCROLL_SPEED;
        if (std::fabs(m_scale + delta) >= 1e-5f)
        {
            m_scale += delta;
        }
    }

    if (!m_focused_sprite_elem)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            on_element_changed.invoke(std::nullopt);
        }

        if (IsKeyDown(KEY_SPACE) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
        {
            m_origin += GetMouseDelta();
        }

        return false;
    }

    if (IsKeyPressed(KeyboardKey::KEY_DELETE))
    {
        remove_scene_element(m_focused_sprite_elem);
        // TODO: should be in remove_scene_element but...
        on_element_removed.invoke(*m_focused_sprite_elem);
        m_focused_sprite_elem = nullptr;
        on_element_changed.invoke(std::nullopt);
        return true;
    }

    if (IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT))
    {
        Vec2 pos = transform_to_canvas(cursor_pos - m_sprite_drag_offset);
        pos = {
            std::floor(pos.x / m_focused_sprite_elem->ppu) * m_focused_sprite_elem->ppu,
            std::floor(pos.y / m_focused_sprite_elem->ppu) * m_focused_sprite_elem->ppu
        }; 
        
        m_focused_sprite_elem->pos = pos;
        on_element_changed.invoke(*m_focused_sprite_elem);
    }

    return false;
}
