#include <iostream>
#include <algorithm>
#include <filesystem>

#include "raygui.h"

#include "Application.h"
#include "AssetExplorer.h"
// #define EDITOR_DEBUG
#include "Utils.h"

// static unsigned char FOLDER_ICON[] = {
//     0b0000'0000'0000'0000,
//     0b0000'0000'0000'0000,
//     0b0111'1110'0000'0000,
//     0b0100'0010'0000'0000,
//     0b0100'0011'1111'1110,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0100'0000'0000'0010,
//     0b0111'1111'1111'1110,
//     0b0000'0000'0000'0000,
//     0b0000'0000'0000'0000
// };


Texture2D FOLDER_TEXTURE;

constexpr auto margin_top { 50 };

AssetExplorer::AssetPreview AssetExplorer::make_dir_preview(const std::filesystem::path& dir)
{
    return AssetExplorer::AssetPreview(std::make_unique<UIImageButton>(
        FOLDER_TEXTURE,
        [this, dir]() {
            open_asset_directory(dir);
        }),
        dir.stem().string()
    );
}

AssetExplorer::AssetPreview AssetExplorer::make_asset_preview(const std::filesystem::path& file)
{
    const auto* am = Core::Application::get().get_asset_manager();
    return AssetExplorer::AssetPreview(std::make_unique<UIImageButton>(
        am->get_asset(file.stem().string()).texture,
        [this, handle = file.stem().string(), am]() {
            add_scene_element.invoke(am->get_asset(handle));
            on_asset_prev_clicked.invoke(am->get_asset(handle));
        }),
        file.stem().string()
    );
}

void AssetExplorer::set_rect(const Rectangle rect)
{
    m_window_rect = rect;
}

// TODO: kann man den root_dir nicht mit im constructor setzen? sollte da schon bekannt sein
//       dann kann open_asset_directory im constructor/init aufgerufen werden
void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    // TODO: just temporary, should be embedded
    FOLDER_TEXTURE = LoadTexture(R"(D:\Mein stuff\Ordner\Privat\Projects\whitespace\example\assets\folder_icon.png)");
    open_asset_directory(root);
}

void AssetExplorer::build_explorer_view(const std::filesystem::path& dir)
{
    m_asset_prevs.clear();

    auto* am = Core::Application::get().get_asset_manager();

    std::vector<AssetPreview> files, dirs;

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_directory())
        {
            dirs.push_back(make_dir_preview(entry));
        }
        else
        {
            const auto path = entry.path();
            if (!am->exists(path.stem().string()))
            {
                am->add_asset(path);
            }
            
            files.push_back(make_asset_preview(entry));
        }
    }

    // join dirs and files -> now ordered
    std::move(files.begin(), files.end(), std::back_inserter(dirs));
    m_asset_prevs = std::move(dirs);
}

void AssetExplorer::render()
{
    GuiGroupBox(m_window_rect, "AssetExplorer");

    if (m_root.empty())
    {
        return;
    }

    draw_asset_previews();
    draw_path_trace();
}

bool AssetExplorer::process_input()
{
    const Vec2 mpos = GetMousePosition();
    const bool left_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    for (const auto& btn : m_asset_prevs | std::views::keys)
    {
        btn->show_bg = false;
        if (btn->is_hovered())
        {
            btn->show_bg = true;
            if (left_clicked)
            {
                btn->on_click();
                return true;
            }
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

    float text_width =
        MeasureTextEx(GuiGetFont(), text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
    
    if (text_width < label_rect.width)
    {
        // center text
        const float offset = (label_rect.width - text_width) / 2.0;
        Rectangle centered_label_rect = label_rect;
        centered_label_rect.x += offset;
        GuiLabel(centered_label_rect, text);
        return;
    }
    else if (text_width > label_rect.width)
    {
        // TODO: this should probably be a general util function -> fit_text_to_rect
        text_width = 0;
        std::string trunc_text; 
        int idx = 0;
        const auto ellipsis_len = MeasureTextEx(GuiGetFont(), "..", GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
        while (text_width < label_rect.width - ellipsis_len)
        {
            trunc_text += text[idx];
            text_width =
                MeasureTextEx(GuiGetFont(), trunc_text.c_str(), GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            idx++;
        }
        trunc_text += "..";
        // DRAW_DEBUG_RECTANGLE(label_rect, BLUE);      
        GuiLabel(label_rect, trunc_text.c_str());
    }
    else 
    {
        GuiLabel(label_rect, text);
    }

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

void AssetExplorer::draw_asset_previews() const
{
    int idx = 0;
    constexpr float preview_size = 80.f;
    constexpr float padding = 10.f;

    for (const auto& [btn, label] : m_asset_prevs)
    {
        const auto rect = place_preview_rect(idx++, preview_size, padding);
        btn->rect = rect;
        btn->render();
        draw_asset_label(btn->rect, label.c_str(), btn->rect.width);
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
