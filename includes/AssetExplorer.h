#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "raylib.h"

#include "Event.h"
#include "AssetManager.h"
#include "UIElements.h"


class AssetExplorer
{
public:
    void set_rect(const Rectangle rect);
    void set_root_dir(const std::filesystem::path& root);
    void render();
    bool process_input();
    Rectangle get_rect() const { return m_window_rect; }

    Core::Event<Core::Asset> add_scene_element;
    Core::Event<const Core::Asset&> on_asset_prev_clicked;

private:
    void build_explorer_view(const std::filesystem::path& root);
    UIButton make_dir_preview(const Rectangle& rect, const std::filesystem::path& dir, float preview_size);
    UIButton make_asset_preview(const Rectangle& rect, const std::filesystem::path& dir, float preview_size);
    Rectangle place_preview_rect(int idx, float preview_size, float padding) const;
    void draw_asset_label(const Rectangle& preview_rect, const char* text, float preview_size) const;
    UIButton* make_path_trace_label(const std::filesystem::path& text);
    void draw_path_trace();
    void open_asset_directory(std::filesystem::path dir);

private:
    std::unordered_map<std::filesystem::path, uint32_t> m_assets;
    std::filesystem::path m_root;
    std::filesystem::path m_current_directory;
    Rectangle m_window_rect;    
    std::vector<UIButton> m_asset_prevs; 
    std::vector<UIButton*> m_ptrace_labels;
};