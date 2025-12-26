#include <iostream>

#include "raygui.h"
#include "raymath.h"

#include "Application.h"
#include "UILayer.h"
#include "CanvasLayer.h"

UILayer::UILayer()
{
    m_inspector.set_rect((Rectangle) {(float)GetScreenWidth() - 200 - 20, 20, 200, 300});
}

void UILayer::init()
{
    Core::Application::get()
        .get_layer<CanvasLayer>()
        ->on_element_changed.add_listener([this](Vector2 pos)
        {
            m_inspector.update_content(pos);
        });
}

void UILayer::update()
{
}

void UILayer::render()
{
    m_inspector.render();
} 

UILayer::~UILayer()
{
}


