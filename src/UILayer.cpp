#include <iostream>
#include <format>
#include <optional>
#include <algorithm>
#include <ranges>

#include "raygui.h"
#include "raymath.h"

#include "Application.h"
#include "UILayer.h"
#include "CanvasLayer.h"
#include "SceneElement.h"

UILayer::UILayer()
{
    m_inspector.set_rect({(float)GetScreenWidth() - 200 - 20, 30, 200, 300});
    m_asset_explorer.set_rect((Rectangle) {100, (float)GetScreenHeight() - 200, 500, 180});
    m_func_ribbon.set_rect((Rectangle) {0, 0, (float)GetScreenWidth(), 20});
    m_hierachy.set_rect({10, 30, 150, 400});
    m_clickable_obj.push_back(m_inspector.get_rect());
    m_clickable_obj.push_back(m_asset_explorer.get_rect());
}

void UILayer::init()
{
    Core::Application& app = Core::Application::get();
    auto* canvas_layer = app.get_layer<CanvasLayer>();

    canvas_layer->on_element_changed.add_listener([this] (const auto& element)
    {
        m_inspector.update_content(element);
    });

    m_asset_explorer.on_asset_prev_clicked.add_listener([this] (const auto& element)
    {
        m_inspector.update_content(element);
    });

    app.get_layer<AppLayer>()
    ->on_project_update.add_listener([this] (const auto& proj_data)
    {
        m_func_ribbon.update_ribbon(proj_data);
    });

    canvas_layer->on_element_added.add_listener([this] (const auto& e)
    {
        m_hierachy.add_entry(e);
    });

    canvas_layer->on_element_removed.add_listener([this] (const auto& e)
    {
        m_hierachy.remove_entry(e);
    });

    canvas_layer->on_element_name_changed.add_listener([this] (const auto& b, const auto& a)
    {
        m_hierachy.update_name(b, a);
    });

    m_hierachy.on_element_selected.add_listener([this, canvas_layer] (const uint16_t id)
    {
        canvas_layer->focus_element(id);
    });
}

void UILayer::update()
{
}

void UILayer::render()
{
    m_inspector.render();
    m_asset_explorer.render();
    m_func_ribbon.render();
    m_hierachy.render();
} 

UILayer::~UILayer()
{
}

void UILayer::set_asset_root(std::filesystem::path path)
{
    m_asset_explorer.set_root_dir(path);
}

bool UILayer::process_input()
{
    bool res = m_asset_explorer.process_input() ||
               m_func_ribbon.process_input()    ||
               m_inspector.process_input()      ||
               m_hierachy.process_input();
    return res;
}
