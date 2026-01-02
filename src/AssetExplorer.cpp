#include <iostream>
#include <algorithm>
#include <filesystem>

#include "raygui.h"

#include "Application.h"
#include "AssetExplorer.h"
#include "Utils.h"

// temporary function until asset data will be saved as well
void AssetExplorer::create_assets()
{
    auto* const am = Core::Application::get().get_asset_manager();
    // TODO: change to recursive_iterator(?) so subdirectories are included 
    for (const auto& entry : std::filesystem::directory_iterator(m_root))
    {
        if (entry.is_directory())
        {
            m_cur_directories.push_back(entry);
            continue;
        }
        else // is "else" really ok here?
        {
            m_cur_files.push_back(entry);
        }

        m_assets[entry] = am->add_asset(entry);
    }
}

void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    m_current_directory = root;
    create_assets();
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

    int idx = 0;
    float preview_size = 80;
    float padding = 10;

    for (const auto& entry : m_cur_directories)
    {
        const auto rect = place_preview_rect(idx, preview_size, padding);
        if (GuiButton(rect, GuiIconText(GuiIconName::ICON_FOLDER, "")))
        {
            open_asset_directory(entry);
            return;
        }
        draw_asset_label(rect, entry.filename().string().c_str(), preview_size);
        idx++;
    }

    for (const auto& entry : m_cur_files)
    {
        const GuiIconName icon = GuiIconName::ICON_FILE;
        const auto rect = place_preview_rect(idx, preview_size, padding);

        if (GuiButton(rect, GuiIconText(icon, "")))
        {
            auto* const am = Core::Application::get().get_asset_manager();
            add_scene_element.invoke(am->get_asset(m_assets[entry]));
        }
        draw_asset_label(rect, entry.filename().string().c_str(), preview_size);
        idx++;
    }
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
    m_cur_directories.clear();
    m_cur_files.clear();

    m_cur_directories.push_back(dir.parent_path());
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_directory())
        {
            m_cur_directories.push_back(entry);
        }
        else // is "else" really ok here?
        {
            m_cur_files.push_back(entry);
        }
    }
}
