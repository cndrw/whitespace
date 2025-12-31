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
    int counter = 0;
    auto* const am = Core::Application::get().get_asset_manager();
    // TODO: change to recursive_iterator(?) so subdirectories are included 
    for (const auto& entry : std::filesystem::directory_iterator(m_root))
    {
        if (entry.is_directory())
        {
            continue;
        }

        m_assets[entry] = am->add_asset(entry);
    }
}

void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    create_assets();
}

void AssetExplorer::set_rect(const Rectangle rect)
{
    m_window_rect = rect;
}

void AssetExplorer::render()
{
    GuiGroupBox(m_window_rect, "AssetExplorer");

    int idx = 0;
    float preview_size = 80;
    float padding = 10;
    for (const auto& e : std::filesystem::directory_iterator(m_root))
    {
        Rectangle rect = {
            m_window_rect.x + (idx * (preview_size + padding) + 10), m_window_rect.y + 10,
            preview_size, preview_size
        };

        idx++;

        const GuiIconName icon = e.is_directory()
            ? GuiIconName::ICON_FOLDER
            : GuiIconName::ICON_FILE;

        if (GuiButton(rect, GuiIconText(icon, "")) && !e.is_directory())
        {
            // auto asset_entry = std::ranges::find_if(m_assets,
            //     [&e](const Asset& a){ return a.path == e.path(); });

            // if (asset_entry != m_assets.end())
            // {
            //     add_scene_element.invoke(*asset_entry);
            // };
            auto* const am = Core::Application::get().get_asset_manager();
            add_scene_element.invoke(am->get_asset(m_assets[e.path()]));
        }

        Rectangle label_rect = {
            rect.x, rect.y + preview_size + 5,
            preview_size, 15
        };

        // DRAW_DEBUG_RECTANGLE(label_rect, BLUE);      
        GuiLabel(label_rect, e.path().filename().string().c_str());
    }
}
