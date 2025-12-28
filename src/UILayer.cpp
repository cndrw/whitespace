#include <iostream>
#include <optional>

#include "raygui.h"
#include "raymath.h"

#include "Application.h"
#include "UILayer.h"
#include "CanvasLayer.h"
#include "SceneElement.h"

UILayer::UILayer()
{
    m_inspector.set_rect((Rectangle) {(float)GetScreenWidth() - 200 - 20, 20, 200, 300});
    m_asset_explorer.set_root_dir("D:\\stuff\\repos\\whitespace\\assets");
    m_asset_explorer.set_rect((Rectangle) {100, (float)GetScreenHeight() - 200, 500, 180});
}

void UILayer::init()
{
    Core::Application::get()
        .get_layer<CanvasLayer>()
        ->on_element_changed.add_listener([this] (const auto& element)
        {
            m_inspector.update_content(element);
        });
}

void UILayer::update()
{
}

void UILayer::render()
{
    m_inspector.render();
    m_asset_explorer.render();
} 

UILayer::~UILayer()
{
}


