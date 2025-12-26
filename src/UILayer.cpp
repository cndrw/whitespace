#include <iostream>

#include "raygui.h"
#include "raymath.h"

#include "Application.h"
#include "UILayer.h"
#include "CanvasLayer.h"

UILayer::UILayer()
{
}

void UILayer::init()
{
    Core::Application::get()
        .get_layer<CanvasLayer>()
        ->on_element_changed.add_listener([this](Vector2 pos)
        {
            update_inspector(pos);
        });
}

void UILayer::update()
{
}

float x_pos = 0;

void UILayer::render()
{
    const float inspector_width = 200;
    const Vector2 inspector_pos = { (float)GetScreenWidth() - inspector_width - 20, 20 };
    GuiGroupBox({ inspector_pos.x, inspector_pos.y, inspector_width, 340 }, "Inspector");

    const auto xlabel_pos = Vector2Add(inspector_pos, { 10, 30 });
    const std::string xlabel = "X: " + std::to_string(x_pos); 
    GuiLabel({ xlabel_pos.x, xlabel_pos.y, 50, 10}, xlabel.c_str());
} 

UILayer::~UILayer()
{
}

// char xlabel_text[32];
// char ylabel_text[32];
// bool edit_mode = false;

// void update_inspector(const Sprite& sprite)
// {
//     const Vector2 inspector_pos = { 20, 20 };
//     GuiGroupBox({ inspector_pos.x, inspector_pos.y, 200, 340 }, "Inspector");

//     const auto xlabel_pos = Vector2Add(inspector_pos, { 10, 30 });
//     const std::string xlabel = "X: " + std::to_string(sprite.rect.x); 
//     GuiLabel({ xlabel_pos.x, xlabel_pos.y, 50, 10}, "X:");

//     const Vector2 cursor_pos = GetMousePosition();
//     if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT) &&
//         CheckCollisionPointRec(cursor_pos, { xlabel_pos.x, xlabel_pos.y, 50, 25}) &&
//         !edit_mode)
//     {
//         edit_mode = true;
//     }
//     int res = GuiTextBox({ xlabel_pos.x + 20, xlabel_pos.y, 50, 25}, xlabel_text, 32, edit_mode);
//     if (res) edit_mode = false;

//     // const auto ylabel_pos = Vector2Add(inspector_pos, { 10, 50 });
//     // const std::string ylabel = "Y: " + std::to_string(sprite.rect.y); 
//     // GuiLabel({ ylabel_pos.x, ylabel_pos.y, 50, 10}, "Y:");

//     // GuiTextBox({ ylabel_pos.x + 20, ylabel_pos.y, 50, 10}, ylabel_text, 32, true);
// }

void UILayer::update_inspector(Vector2 pos)
{
    x_pos = pos.x;
}

