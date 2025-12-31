#pragma once

#include <filesystem>

#include "yaml-cpp/yaml.h"

#include "Layer.h"

class DataPersitanceLayer : public Core::Layer
{
public:
    DataPersitanceLayer(std::filesystem::path root) : m_root(std::move(root)) {}
    // virtual void init() override;
    // virtual void update() override;
    // virtual void render() override;
    void save_scene(const YAML::Node& scene);
    YAML::Node load_scene(const std::filesystem::path& file);
    virtual ~DataPersitanceLayer() {}

private:
    std::filesystem::path m_root;
};