#include <iostream> 

#include "AssetManager.h"

#include "raylib.h"

namespace Core
{
    uint32_t AssetManager::add_asset(const std::filesystem::path& path)
    {
        std::cout << "adde new asset\n";
        m_current_id++;
        m_assets[m_current_id] = (Asset) {
            .path = path,
            .texture = LoadTexture(path.string().c_str()),
            .id = m_current_id
        };

        return m_current_id;
    }

    Asset AssetManager::get_asset(const uint32_t id)
    {
        return m_assets.at(id);
    }
}