#pragma once

#include <filesystem>

#include "yaml-cpp/yaml.h"

#include "Layer.h"

class DataPersitanceLayer : public Core::Layer
{
public:
    void set_saves_root(std::filesystem::path root);
    void save_scene(const YAML::Node& scene);
    YAML::Node load_scene(const std::filesystem::path& file);
    virtual ~DataPersitanceLayer() {}

private:
    std::filesystem::path m_root;
};