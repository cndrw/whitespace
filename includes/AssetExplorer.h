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
    std::unordered_map<std::filesystem::path, uint32_t> m_assets;
    std::filesystem::path m_root;
    Rectangle m_window_rect;    
};