#include <iostream> 

#include "AssetManager.h"

#include "raylib.h"

namespace Core
{
    void AssetManager::add_asset(const std::filesystem::path& path)
    {
        std::cout << "Adding asset: " << path.stem() << std::endl;
        m_assets[path.stem().string()] = (Asset) {
            .path = path,
            .texture = LoadTexture(path.string().c_str()),
            .ppu = 32 // TODO: set default back to 100
        };
    }

    Asset AssetManager::get_asset(const Handle& handle) const
    {
        std::cout << "Getting asset: " << handle << std::endl;
        return m_assets.at(handle);
    }

    bool AssetManager::exists(const Handle &handle) const
    {
        return m_assets.contains(handle);
    }
}