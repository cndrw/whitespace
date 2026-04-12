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
        std::filesystem::path rel_path;
        std::string name;
        Texture2D texture;
    } Asset;

    class AssetManager
    {
    public:
        void add_asset(const std::filesystem::path& path);
        Asset add_asset_from_relative(std::filesystem::path path);
        Asset get_asset(const Handle& handle) const;
        void update_asset(const Asset& asset);
        bool exists(const Handle& handle) const;
        inline void set_root(std::filesystem::path dir) { m_root = dir; }

        Core::Event<const Handle&> on_asset_update;

    private:
        std::unordered_map<Handle, Asset> m_assets;
        std::filesystem::path m_root;
        uint32_t m_current_id = 0;
    };
}
