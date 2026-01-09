#pragma once

#include <unordered_map>
#include <filesystem>
#include <memory>

#include "raylib.h"

namespace Core
{
    typedef struct {
        std::filesystem::path path;
        Texture2D texture;
        uint32_t ppu;
        uint32_t id;
    } Asset;

    class AssetManager
    {
    public:
        uint32_t add_asset(const std::filesystem::path& path);
        Asset get_asset(const uint32_t id);
    private:
        std::unordered_map<uint32_t, Asset> m_assets;
        uint32_t m_current_id = 0;
    };
}
