#pragma once

#include <filesystem>

#include "raylib.h"

#include "Event.h"

class AssetExplorer
{
public:
    void set_rect(const Rectangle rect);
    void set_root_dir(const std::filesystem::path& root);
    void render();

public:
    Core::Event<std::filesystem::path> add_scene_element;

private:
    std::filesystem::path m_root;
    Rectangle m_window_rect;    
};