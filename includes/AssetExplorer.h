#pragma once

#include <filesystem>
#include <unordered_map>

#include "raylib.h"

#include "Event.h"
#include "AssetManager.h"


class AssetExplorer
{
public:
    void set_rect(const Rectangle rect);
    void set_root_dir(const std::filesystem::path& root);
    void create_assets();
    void render();

public:
    Core::Event<Core::Asset> add_scene_element;

private:
    Rectangle place_preview_rect(int idx, float preview_size, float padding) const;
    void draw_asset_label(const Rectangle& preview_rect, const char* text, float preview_size) const;
    void open_asset_directory(std::filesystem::path dir);

private:
    std::unordered_map<std::filesystem::path, uint32_t> m_assets;
    std::filesystem::path m_root;
    std::filesystem::path m_current_directory;
    std::vector<std::filesystem::path> m_cur_directories;
    std::vector<std::filesystem::path> m_cur_files;
    Rectangle m_window_rect;    
};