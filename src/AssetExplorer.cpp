#include <iostream>
#include <algorithm>
#include <filesystem>

#include "raygui.h"

#include "Application.h"
#include "AssetExplorer.h"
// #define EDITOR_DEBUG
#include "Utils.h"

constexpr auto margin_top { 50 };

UIButton AssetExplorer::make_dir_preview(
    const Rectangle& rect, const std::filesystem::path& dir, const float preview_size)
{
    return UIButton(
        rect,
        [this, rect, preview_size, name = dir.filename().string()]() {
            GuiButton(rect, GuiIconText(ICON_FOLDER, ""));
            draw_asset_label(rect, name.c_str(), preview_size);
        },
        [this, dir]() {
            open_asset_directory(dir);
        }
    );
}

UIButton AssetExplorer::make_asset_preview(
    const Rectangle& rect, const std::filesystem::path& file, const float preview_size)
{
    return UIButton(
        rect,
        [this, rect, preview_size, name = file.filename().string()]() {
            GuiButton(rect, GuiIconText(ICON_FILE, ""));
            draw_asset_label(rect, name.c_str(), preview_size);
        },
        [this, name = file.stem().string()]() {
            auto* const am = Core::Application::get().get_asset_manager();
            add_scene_element.invoke(am->get_asset(name));
            on_asset_prev_clicked.invoke(am->get_asset(name));
        }
    );
}

// std::shared_ptr<AssetPreview> AssetExplorer::make_asset_prev(
//     const Rectangle& rect, const std::filesystem::path& file, const float preview_size)
// {
//     const auto* am = Core::Application::get().get_asset_manager();
//     return std::make_shared<AssetPreview>({
//         rect,
//         am->get_asset(file.stem().string()),
//         [this, name = file.stem().string(), am]() {
//             add_scene_element.invoke(am->get_asset(name));
//             on_asset_prev_clicked.invoke(am->get_asset(name));
//         }},
//         file.stem().string()
//     );
// }

void AssetExplorer::set_rect(const Rectangle rect)
{
    m_window_rect = rect;
}

// TODO: kann man den root_dir nicht mit im constructor setzen? sollte da schon bekannt sein
//       dann kann open_asset_directory im constructor/init aufgerufen werden
void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    open_asset_directory(root);
}

void AssetExplorer::build_explorer_view(const std::filesystem::path& dir)
{
    m_asset_prevs.clear();

    int idx = 0;
    constexpr float preview_size = 80.f;
    constexpr float padding = 10.f;

    auto* am = Core::Application::get().get_asset_manager();

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        const auto rect = place_preview_rect(idx++, preview_size, padding);

        if (entry.is_directory())
        {
            m_asset_prevs.push_back(make_dir_preview(rect, entry, preview_size));
        }
        else
        {
            const auto path = entry.path();
            if (!am->exists(path.stem().string()))
            {
                am->add_asset(path);
            }
            
            m_asset_prevs.push_back(make_asset_preview(rect, entry, preview_size));
        }
    }
}

void AssetExplorer::render()
{
    GuiGroupBox(m_window_rect, "AssetExplorer");

    if (m_root.empty())
    {
        return;
    }

    for (const auto& prev : m_asset_prevs)
    {
        prev.render();
    }

    draw_path_trace();
}

bool AssetExplorer::process_input()
{
    const Vec2 mpos = GetMousePosition();
    const bool left_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    for (const auto& prev : m_asset_prevs)
    {
        if (CheckCollisionPointRec(mpos, prev.rect) && left_clicked)
        {
            prev.on_click();
            return true;
        }
    }

    for (const auto& label : m_ptrace_labels)
    {
        if (CheckCollisionPointRec(mpos, label->rect) && left_clicked)
        {
            label->on_click();
            return true;
        }
    }

    return CheckCollisionPointRec(mpos, m_window_rect);
}

Rectangle AssetExplorer::place_preview_rect(int idx, float preview_size, float padding) const
{
    return {
        .x = m_window_rect.x + (idx * (preview_size + padding) + 10),
        .y = m_window_rect.y + margin_top,
        .width = preview_size,
        .height = preview_size
    };
}

void AssetExplorer::draw_asset_label(
    const Rectangle& preview_rect, const char* text, const float preview_size) const
{
    const Rectangle label_rect = {
        .x = preview_rect.x,
        .y = preview_rect.y + preview_size + 5,
        .width = preview_size,
        .height = 15
    };

    // DRAW_DEBUG_RECTANGLE(label_rect, BLUE);      
    GuiLabel(label_rect, text);
}

void AssetExplorer::draw_path_trace()
{
    constexpr float label_spacing = 15;
    float label_width = 100;
    float x_offset = 10;

    auto& labels = m_ptrace_labels;
    for (size_t i = 0; i < labels.size(); i++)
    {
        // draw parts as label buttons
        std::string text = labels[i]->text;
        label_width = MeasureTextEx(GuiGetFont(), text.c_str(), GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;

        Rectangle rect = {
            .x = m_window_rect.x + 10 + (x_offset),
            .y = m_window_rect.y + 20,
            .width = label_width,
            .height = 20
        };

        labels[i]->rect = rect;

        DRAW_DEBUG_RECTANGLE(rect, GREEN);
        labels[i]->render();

        x_offset += label_width + label_spacing;

        if (i != labels.size() - 1)
        {
            // draw separator
            Rectangle rect = {
                .x = m_window_rect.x + 10 + (x_offset - label_spacing) + 4,
                .y = m_window_rect.y + 20,
                .width = label_spacing,
                .height = 20
            };
            DRAW_DEBUG_RECTANGLE(rect, RED);
            GuiLabel(rect, ">"); // apparently this style (font?) cannot display ">" correctly
        }
    }
}

std::unique_ptr<UIButton> AssetExplorer::make_path_trace_label(const std::filesystem::path& path)
{
    return std::make_unique<UIButton>(
        Rectangle{},
        [this, path]() {
            if (path == m_current_directory) return;
            open_asset_directory(path);
        },
        path.stem().string()
    );
}


void AssetExplorer::open_asset_directory(std::filesystem::path dir)
{
    m_current_directory = dir;
    build_explorer_view(m_current_directory);

    // update path trace
    std::filesystem::path trace = m_current_directory;
    m_ptrace_labels.clear();
    m_ptrace_labels.push_back(make_path_trace_label(trace));

    while (trace != m_root && trace.has_parent_path())
    {
        trace = trace.parent_path();
        m_ptrace_labels.push_back(make_path_trace_label(trace));
    }

    std::ranges::reverse(m_ptrace_labels);
}
