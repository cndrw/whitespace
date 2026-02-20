#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>
#include <memory>

#include "raylib.h"

#include "Event.h"

using Handle = std::string;

namespace Core
{
    typedef struct {
        std::filesystem::path path;
        std::string name;
        Texture2D texture;
        uint32_t ppu;
    } Asset;

    class AssetManager
    {
    public:
        void add_asset(const std::filesystem::path& path);
        Asset get_asset(const Handle& handle) const;
        void update_asset(const Asset& asset);
        bool exists(const Handle& handle) const;

        Core::Event<const Handle&> on_asset_update;

    private:
        std::unordered_map<Handle, Asset> m_assets;
        uint32_t m_current_id = 0;
    };
}
