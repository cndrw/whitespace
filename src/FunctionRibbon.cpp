#include <memory>
#include <iostream>
#include <format>

#include "raygui.h"

#include "FunctionRibbon.h"
#include "Application.h"
#include "AppLayer.h"

FunctionRibbon::FunctionRibbon()
{
    m_buttons = {
        make_button({ 250, 0, 50, 20 }, "#005#Save", []() {
            std::cout << "Current loaded save" << std::endl;
        }),
        make_button({ 100, 0, 50, 20 }, "#005#Open", []() {
            std::cout << "Open button clicked!" << std::endl;
        }),
        make_button({ 0, 0, 50, 20 }, "#006#Load", []() {
            Core::Application::get().get_layer<AppLayer>()->open_project();
        })
    };
}

bool FunctionRibbon::process_input()
{
    Vector2 mouse_pos = GetMousePosition();

    for (const auto& button : m_buttons)
    {
        if (CheckCollisionPointRec(mouse_pos, button.rect))
        {
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                button.on_click();
                return true;
            }
        }
    }

    return CheckCollisionPointRec(mouse_pos, m_rect);
}

UIElement FunctionRibbon::make_button(const Rectangle &rect, const char *label, const std::function<void()> &on_click)
{
    return (UIElement){
        .rect = rect,
        .on_click = on_click,
        .render = [rect, label]() {
            GuiLabelButton(rect, label);
        }
    };
}

void FunctionRibbon::update_ribbon(const ProjectMetadata& proj_data)
{
    constexpr auto scene_label_idx { 0 };
    constexpr auto proj_label_idx { 1 };

    m_buttons[proj_label_idx].render =
        [rect = m_buttons[proj_label_idx].rect, proj_name = proj_data.project_name]() {
            GuiLabelButton(rect, std::format("Project: {}", proj_name).c_str());
    };

    m_buttons[scene_label_idx].render =
        [rect = m_buttons[scene_label_idx].rect, scene_name = proj_data.active_scene_name.c_str()]() {
            GuiLabelButton(rect, scene_name);
    };
}

void FunctionRibbon::render()
{
    for (const auto& button : m_buttons)
    {
        button.render();
    }
}


void FunctionRibbon::set_rect(Rectangle rect)
{
    m_rect = std::move(rect);
}
