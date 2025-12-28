#include <iostream>

#include "raygui.h"

#include "AssetExplorer.h"
#include "Utils.h"

void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
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
            add_scene_element.invoke(e);
        }

        Rectangle label_rect = {
            rect.x, rect.y + preview_size + 5,
            preview_size, 15
        };

        // DRAW_DEBUG_RECTANGLE(label_rect, BLUE);      
        GuiLabel(label_rect, e.path().filename().string().c_str());
    }
}
