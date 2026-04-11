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
    : UIComponent{"Function Ribbon"}
{
    constexpr auto win_height = 150;
    constexpr auto win_width  = 450;
    const Vec2 screen_mid = Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };

    m_open_project_window = std::make_unique<OpenProjectWindow>((Rectangle) {
        screen_mid.x - win_width / 2,
        screen_mid.y - win_height / 2 - 100,
        win_width,
        win_height
    });

    m_create_project_window = std::make_unique<CreateProjectWindow>((Rectangle) {
        screen_mid.x - win_width / 2,
        screen_mid.y - win_height / 2 - 100,
        win_width,
        win_height + 90
    });

    m_scene_static_label = { 225, 0, 50, 20 };
    m_buttons = {
        new UIDropDownList(
            { 0, 0, 50, 20 },
            "#005#File",
            UIDropDownList::ExpansionType::DOWN
        ),
        new UIDropDownList({
            m_scene_static_label.x + m_scene_static_label.width, 0, 50, 20 },
            "#005#Open",
            UIDropDownList::ExpansionType::DOWN
        ),
        make_button({ 100, 0, 50, 20 }, "#005#Save", []() {
            std::cout << "Current loaded save" << std::endl;
        })
    };

    m_buttons[1]->text = "scene_button";
    m_buttons[2]->text = "save_button";
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

void FunctionRibbon::init()
{
    const auto recent_projects = Core::Application::get().get_layer<AppLayer>()->get_recent_projects();
    std::cout << "received: " << recent_projects.size() << std::endl;

    auto* file_label = dynamic_cast<UIDropDownList*>(m_buttons[0]);

    // Spaces just that the aligment works for now...
    auto& sub_dp_list = file_label->add_nested("Open Recent..   ", UIDropDownList::ExpansionType::SIDE);
    for (auto& rp : recent_projects)
    {
        const auto name = rp.stem().string();
        sub_dp_list.add_item(name, [rp] { Core::Application::get().get_layer<AppLayer>()->open_project(rp.parent_path()); });
    }

    file_label->add_item("Open Project", [this] {
        m_open_project_window->open();
    });

    file_label->add_item("Create Project", [this] {
        m_create_project_window->open();
    });
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

    return CheckCollisionPointRec(mouse_pos, m_outer_rect) || m_open_project_window->process_input() || m_create_project_window->process_input();
        
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
    auto* scene_label = dynamic_cast<UIDropDownList*>(m_buttons[1]);
    auto& proj_label = m_buttons[2];

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

    if (!proj_data.scene_list.empty())
    {
        const auto& active_scene = proj_data.scene_list[proj_data.active_scene_idx];
        scene_label->set_header(active_scene.scene_name);

        auto other_scenes = proj_data.scene_list;
        const auto it = std::ranges::find_if(other_scenes, [active_scene](const auto& data) { return data.scene_name == active_scene.scene_name; });
        other_scenes.erase(it);

        scene_label->clear_items();
        for (auto& item : other_scenes)
        {
            scene_label->add_item(item.scene_name, [scene_name = item.scene_name]() {
                Core::Application::get().get_layer<AppLayer>()->load_scene(scene_name);
            });
        }
    }
}

void FunctionRibbon::render()
{
    DrawRectangleRec(m_outer_rect, m_bg_color);
    for (const auto& button : m_buttons)
    {
        button->render();
    }

    for (const auto& label : m_labels)
    {
        label->render();
    }

    m_open_project_window->render();
    m_create_project_window->render();
}


