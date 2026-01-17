#include <memory>
#include <iostream>
#include <ranges>
#include <format>
#include <string>
#include <vector>

#include "raygui.h"

#include "FunctionRibbon.h"
#include "Application.h"
#include "AppLayer.h"
#include "CanvasLayer.h"
// #define EDITOR_DEBUG 
#include "Utils.h"

FunctionRibbon::FunctionRibbon()
{
    m_scene_static_label = { 225, 0, 50, 20 };
    m_buttons = {
        new UIDropDownList({
            m_scene_static_label.x + m_scene_static_label.width, 0, 50, 20 },
            "#005#Open"),
        make_button({ 100, 0, 50, 20 }, "#005#Save", []() {
            std::cout << "Current loaded save" << std::endl;
        }),
        make_button({ 0, 0, 50, 20 }, "#006#Load", []() {
            Core::Application::get().get_layer<AppLayer>()->open_project();
        })
    };
}

FunctionRibbon::~FunctionRibbon()
{
    for (auto& button : m_buttons)
    {
        delete button;
    }

    for (auto& label : m_labels)
    {
        delete label;
    }
}

bool FunctionRibbon::process_input()
{
    Vector2 mouse_pos = GetMousePosition();


    for (const auto& button : m_buttons)
    {
        if (button->is_hovered())
        {
            button->on_hover();
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                button->on_click();
                return true;
            }
        }
    }

    return CheckCollisionPointRec(mouse_pos, m_rect);
        
}

UIButton* FunctionRibbon::make_button(
    const Rectangle &rect,
    const char *label,
    const std::function<void()>& on_click
)
{
    return new UIButton(
        rect,
        [rect, label]() {
            GuiLabelButton(rect, label);
        },
        on_click
    );
}

void FunctionRibbon::update_ribbon(const ProjectMetadata& proj_data)
{
    auto* scene_label = dynamic_cast<UIDropDownList*>(m_buttons[0]);
    auto& proj_label = m_buttons[1];

    proj_label->render =
        [rect = proj_label->rect, proj_name = proj_data.project_name]() {
            GuiLabelButton(rect, std::format("Project: {}", proj_name).c_str());
    };

    m_labels.push_back(
        new UIElement(
            m_scene_static_label,
            [this]() {
                GuiLabel(m_scene_static_label, "Scene:");
            }
        )
    );

    const auto& active_scene = proj_data.scene_list[proj_data.active_scene_idx];
    scene_label->set_header(active_scene);

    auto other_scenes = proj_data.scene_list;
    const auto it = std::ranges::find(other_scenes, active_scene);
    other_scenes.erase(it);

    scene_label->clear_items();
    for (auto& item : other_scenes)
    {
        scene_label->add_item(item, [scene_name = item]() {
            Core::Application::get().get_layer<AppLayer>()->load_scene(scene_name);
        });
    }
}

void FunctionRibbon::render()
{
    for (const auto& button : m_buttons)
    {
        button->render();
    }

    for (const auto& label : m_labels)
    {
        label->render();
    }
}


void FunctionRibbon::set_rect(Rectangle rect)
{
    m_rect = std::move(rect);
}
