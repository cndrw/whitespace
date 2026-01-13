#include <iostream>
#include <algorithm>
#include <filesystem>

#include "raygui.h"

#include "Application.h"
#include "AssetExplorer.h"
#include "Utils.h"

UIButton AssetExplorer::make_dir_preview(
    const Rectangle& rect, const std::filesystem::path& dir, float preview_size
)
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
    const Rectangle& rect, const std::filesystem::path& file, float preview_size
)
{
    return UIButton(
        rect,
        [this, rect, preview_size, name = file.filename().string()]() {
            GuiButton(rect, GuiIconText(ICON_FILE, ""));
            draw_asset_label(rect, name.c_str(), preview_size);
        },
        [this, file]() {
            auto* const am = Core::Application::get().get_asset_manager();
            add_scene_element.invoke(am->get_asset(m_assets[file]));
            on_asset_prev_clicked.invoke(am->get_asset(m_assets[file]));
        }
    );
}

void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    m_current_directory = root;
    build_explorer_view(m_current_directory);
}

void AssetExplorer::build_explorer_view(const std::filesystem::path& dir)
{
    m_asset_prevs.clear();

    int idx = 0;
    constexpr float preview_size = 80.f;
    constexpr float padding = 10.f;

    auto* am = Core::Application::get().get_asset_manager();

    m_asset_prevs.push_back(
        make_dir_preview(place_preview_rect(idx++, preview_size, padding), dir.parent_path(), preview_size)
    );

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        const auto rect = place_preview_rect(idx++, preview_size, padding);

        if (entry.is_directory())
        {
            m_asset_prevs.push_back(make_dir_preview(rect, entry, preview_size));
        }
        else
        {
            m_assets.try_emplace(entry, am->add_asset(entry));
            m_asset_prevs.push_back(make_asset_preview(rect, entry, preview_size));
        }
    }
}

void AssetExplorer::set_rect(const Rectangle rect)
{
    m_window_rect = rect;
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
}

bool AssetExplorer::process_input()
{
    const Vector2 mpos = GetMousePosition();

    for (const auto& prev : m_asset_prevs)
    {
        if (CheckCollisionPointRec(mpos, prev.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            prev.on_click();
            return true;
        }
    }

    return false;
}

Rectangle AssetExplorer::place_preview_rect(int idx, float preview_size, float padding) const
{
    return {
        .x = m_window_rect.x + (idx * (preview_size + padding) + 10),
        .y = m_window_rect.y + 10,
        .width = preview_size,
        .height = preview_size
    };
}

void AssetExplorer::draw_asset_label(
    const Rectangle& preview_rect, const char* text, const float preview_size
) const
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

void AssetExplorer::open_asset_directory(std::filesystem::path dir)
{
    m_current_directory = dir;
    build_explorer_view(m_current_directory);
}
