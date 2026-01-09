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
    m_inspector.set_rect((Rectangle) {(float)GetScreenWidth() - 200 - 20, 20, 200, 300});
    m_asset_explorer.set_rect((Rectangle) {100, (float)GetScreenHeight() - 200, 500, 180});
    m_func_ribbon.set_rect((Rectangle) {0, 0, (float)GetScreenWidth(), 20});
    m_clickable_obj.push_back(m_inspector.get_rect());
    m_clickable_obj.push_back(m_asset_explorer.get_rect());
}

void UILayer::init()
{
    auto canvas_layer = Core::Application::get().get_layer<CanvasLayer>();
    canvas_layer->on_element_changed.add_listener([this] (const auto& element)
    {
        m_inspector.update_content(element);
    });

    m_asset_explorer.on_asset_prev_clicked.add_listener([this] (const auto& element)
    {
        m_inspector.update_content(element);
    });

    Core::Application::get()
        .get_layer<AppLayer>()
        ->on_project_update.add_listener([this] (const auto& proj_data)
        {
            m_func_ribbon.update_ribbon(proj_data);
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
    bool res = m_asset_explorer.process_input() || m_func_ribbon.process_input();
    return res;
}
