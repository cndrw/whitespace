#pragma once

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>

#include "raylib.h"

#include "Event.h"
#include "AssetManager.h"
#include "UIElements.h"
#include "Utils.h"


class AssetExplorer : public UIComponent
{
public:
    struct AssetPreview
    {
        std::unique_ptr<UIImageButton> button;
        std::string label;
        bool active;
    };

    AssetExplorer() : UIComponent{"Asset Explorer"} {} 
    void set_root_dir(const std::filesystem::path& root);
    bool process_input();
    Rectangle get_rect() const { return m_outer_rect; }

    Core::Event<Core::Asset> add_scene_element;
    Core::Event<const Core::Asset&> on_asset_prev_clicked;

protected:
    void render_impl() override;

private:
    void build_explorer_view(const std::filesystem::path& root);
    std::unique_ptr<UIButton> make_path_trace_label(const std::filesystem::path& text);
    AssetPreview make_asset_preview(const std::filesystem::path& file);
    AssetPreview make_dir_preview(const std::filesystem::path& dir);

    void open_asset_directory(std::filesystem::path dir);
    Rectangle place_preview_rect(int idx, float preview_size, float padding) const;
    void draw_asset_label(const Rectangle& preview_rect, const char* text, float preview_size) const;
    void draw_path_trace();
    void draw_asset_previews() const;
    void draw_drag_action_indicator() const;

    void generate_layout();
    std::vector<Vec2> generate_layout_template(float preview_size, size_t size) const;
    void handle_scrolling();
    bool check_if_in_view(const AssetPreview& entry) const;

private:
    std::vector<std::unique_ptr<UIButton>> m_ptrace_labels;
    std::vector<AssetPreview> m_asset_prevs;
    std::filesystem::path m_current_directory;
    std::filesystem::path m_root;
    std::string m_selected_preview;
    Rectangle m_asset_prev_rect;
    bool m_drag_action = false;
};