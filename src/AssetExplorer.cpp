#include <iostream>
#include <algorithm>
#include <filesystem>

#include "raygui.h"

#include "Application.h"
#include "AssetExplorer.h"
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

AssetExplorer::AssetPreview AssetExplorer::make_dir_preview(const std::filesystem::path& dir)
{
    return AssetPreview{
        .button = std::make_unique<UIImageButton>(
            FOLDER_TEXTURE,
            [this, dir] { open_asset_directory(dir); }),
        .label = dir.stem().string(),
        .active = false
    };
}

AssetExplorer::AssetPreview AssetExplorer::make_asset_preview(const std::filesystem::path& file)
{
    const auto* am = Core::Application::get().get_asset_manager();
    const auto handle = file.stem().string();

    return AssetPreview{
        .button = std::make_unique<UIImageButton>(
            am->get_asset(handle).texture,
            [this, handle, am] {
                on_asset_prev_clicked.invoke(am->get_asset(handle));
                m_selected_preview = handle; 
                m_drag_action = true;
            }),
        .label = file.stem().string(),
        .active = false
    };
}

// TODO: kann man den root_dir nicht mit im constructor setzen? sollte da schon bekannt sein
//       dann kann open_asset_directory im constructor/init aufgerufen werden
void AssetExplorer::set_root_dir(const std::filesystem::path& root)
{
    m_root = root;
    // TODO: just temporary, should be embedded
    // FOLDER_TEXTURE = LoadTexture(R"(D:\Mein stuff\Ordner\Privat\Projects\whitespace\example\assets\folder_icon.png)");
    FOLDER_TEXTURE = LoadTexture(R"(D:\stuff\repos\whitespace\example\assets\folder_icon.png)");
    
    // TOOD: should also not be here, tbd when initialization is reworked for UIComponents
    static constexpr auto margin_top   { 50 };
    static constexpr auto margin_left  { 10 };

    m_asset_prev_rect = m_inner_rect;
    m_asset_prev_rect.y += margin_top;
    m_asset_prev_rect.x += margin_left;
    m_asset_prev_rect.height -= margin_top + margin_left;
    m_asset_prev_rect.width -= margin_left * 2;

    open_asset_directory(root);
}

void AssetExplorer::build_explorer_view(const std::filesystem::path& dir)
{
    static constexpr std::array<const char*, 2> VALID_EXTENSIONS = { ".png", ".jpg" };
    m_asset_prevs.clear();

    auto* am = Core::Application::get().get_asset_manager();

    std::vector<AssetPreview> files, dirs;

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_directory())
        {
            dirs.push_back(make_dir_preview(entry));
        }
        else if (std::ranges::find(VALID_EXTENSIONS, entry.path().extension()) != VALID_EXTENSIONS.end())
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

void AssetExplorer::render_impl()
{
    if (m_root.empty())
    {
        return;
    }

    DRAW_DEBUG_RECTANGLE(m_asset_prev_rect, BLUE);

    draw_asset_previews();
    draw_path_trace();
    draw_drag_action_indicator();

}

bool AssetExplorer::process_input()
{
    const Vec2 mpos = GetMousePosition();
    const bool left_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    handle_scrolling();

    if (m_drag_action && !CheckCollisionPointRec(mpos, m_outer_rect))
    {
        m_drag_action = false;
        add_scene_element.invoke(
            Core::Application::get().get_asset_manager()->get_asset(m_selected_preview)
        );
    }

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        m_drag_action = false;
    }

    if (!m_drag_action)
    {
        for (const auto& prev : m_asset_prevs)
        {
            prev.button->show_bg = false;
            if (prev.button->is_hovered())
            {
                prev.button->show_bg = true;
                if (left_clicked)
                {
                    prev.button->on_click();
                    return true;
                }
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

    return CheckCollisionPointRec(mpos, m_outer_rect);
}

Rectangle AssetExplorer::place_preview_rect(int idx, float preview_size, float padding) const
{
    static constexpr auto margin_top { 50 };
    return {
        .x = m_inner_rect.x + (idx * (preview_size + padding) + 10),
        .y = m_inner_rect.y + margin_top,
        .width = preview_size,
        .height = preview_size
    };
}

void AssetExplorer::draw_asset_label(const Rectangle& preview_rect, const char* text, const float preview_size) const
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
        int idx = 0;
        std::string trunc_text; 

        const int gui_txt_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
        const int gui_txt_spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
        const Font font = GuiGetFont();

        const auto ellipsis_len = MeasureTextEx(font, "..", gui_txt_size, gui_txt_spacing).x;
        while (text_width < label_rect.width - ellipsis_len)
        {
            trunc_text += text[idx];
            text_width = MeasureTextEx(font, trunc_text.c_str(), gui_txt_size, gui_txt_spacing).x;
            idx++;
        }
        trunc_text.pop_back();
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
            .x = m_inner_rect.x + 10 + (x_offset),
            .y = m_inner_rect.y + 20,
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
                .x = m_inner_rect.x + 10 + (x_offset - label_spacing) + 4,
                .y = m_inner_rect.y + 20,
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
    for (const auto& prev : m_asset_prevs)
    {
        if (prev.active)
        {
            prev.button->render();
            draw_asset_label(prev.button->rect, prev.label.c_str(), prev.button->rect.width);
        }
    }
}

void AssetExplorer::draw_drag_action_indicator() const
{
    if (!m_drag_action) return;

    static constexpr Vec2 offset = { 20, 10 };
    const Vec2 mpos = GetMousePosition();
    const Rectangle rect = {
        .x = mpos.x - offset.x / 2.0f,
        .y = mpos.y - offset.y / 2.0f,
        .width = offset.x,
        .height = offset.y
    };

    DrawRectangleRec(rect, Color { 254, 254, 254, 50 });
    DrawRectangleLinesEx(rect, 1.0f, RAYWHITE);
}

void AssetExplorer::generate_layout()
{
    static constexpr auto preview_size { 80 };

    std::vector<Vec2> layout = generate_layout_template(preview_size, m_asset_prevs.size());
    int idx = 0;

    for (auto& prev : m_asset_prevs)
    {
        prev.button->set_pos(layout[idx++]);
        prev.button->rect.height = preview_size;
        prev.button->rect.width = preview_size;
        prev.active = check_if_in_view(prev);
    }
}

std::vector<Vec2> AssetExplorer::generate_layout_template(const float preview_size, const size_t size) const
{
    std::vector<Vec2> output;
    output.reserve(size);

    static constexpr auto padding { 22 };

    int x_idx = 0;
    int y_idx = 0;

    const float step = preview_size + padding;

    for (size_t i = 0; i < size; i++)
    {
        const float x_pos = m_asset_prev_rect.x + x_idx * step;

        if (x_pos / (m_asset_prev_rect.x + m_asset_prev_rect.width - preview_size) > 1)
        {
            x_idx = 0;
            y_idx++;
        }

        output.push_back({
            m_asset_prev_rect.x + x_idx * step,
            m_asset_prev_rect.y + y_idx * step
        });

        x_idx++;
    }

    return output;
}

void AssetExplorer::handle_scrolling()
{
    static constexpr auto SCROLL_SPEED { 10 };
    static constexpr auto LABEL_HEIGHT { 15 }; // TODO: sollte auch nicht hier sein
    static constexpr auto SPACING { 5 };       // TODO: sollte auch nicht hier sein
    float scroll_y = 0;

    if (CheckCollisionPointRec(GetMousePosition(), m_asset_prev_rect))
    {
        scroll_y = GetMouseWheelMoveV().y;
    }

    if (!m_asset_prevs.empty())
    {
        int8_t sign = 0;
        if (scroll_y > 0 && m_asset_prevs.front().button->rect.y < m_asset_prev_rect.y)
        {
            sign = 1;
        }
        else if (scroll_y < 0 && m_asset_prevs.back().button->rect.y + m_asset_prevs.back().button->rect.height + LABEL_HEIGHT + SPACING > m_asset_prev_rect.y + m_asset_prev_rect.height)
        {
            sign = -1;
        }

        for (auto& prev : m_asset_prevs)
        {
            prev.button->rect.y += sign * SCROLL_SPEED;
            prev.active = check_if_in_view(prev);
        }
    }
}

bool AssetExplorer::check_if_in_view(const AssetPreview &entry) const
{
    auto& r = entry.button->rect;
    return r.y + r.height <= m_asset_prev_rect.y + m_asset_prev_rect.height && r.y >= m_asset_prev_rect.y;
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

    generate_layout();

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
