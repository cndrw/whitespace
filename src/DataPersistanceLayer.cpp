#include <iostream>
#include <format>
#include <fstream>

#include "Application.h"
#include "AppLayer.h"
#include "DataPersistanceLayer.h"

void DataPersitanceLayer::set_saves_root(std::filesystem::path root)
{
    m_root = root;
}

void DataPersitanceLayer::save_scene(const YAML::Node &scene)
{

    auto* app_layer = Core::Application::get().get_layer<AppLayer>();
    auto scene_data = app_layer->get_cur_scene();

    std::filesystem::path dest_path;
    if (scene_data.has_value())
    {
        dest_path = scene_data->full_path().replace_extension(".wsscene");
    }
    else
    {
        const SceneData unnamed_scene = {
            .scene_name = "unnamed",
            .save_dir = m_root
        };
        dest_path = unnamed_scene.full_path().replace_extension(".wsscene");
        app_layer->add_scene(std::move(unnamed_scene));
    }

    std::cout << std::format("Saving {}", dest_path.string());
    std::ofstream fout(dest_path);
    fout << scene;
    std::cout << std::format("Saved in: {}\n", dest_path.string());
}

YAML::Node DataPersitanceLayer::load_scene(const std::filesystem::path& file)
{
    // TODO: das ist nur temporär
    std::cout << std::format("Try Load File: {}\n", file.string());
    return YAML::LoadFile(file.string());
}
