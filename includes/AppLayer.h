#pragma once

#include <filesystem>

#include "Layer.h"
#include "Event.h"

typedef struct {
    std::string project_name;
    std::string active_scene_name;
} ProjectMetadata;

class AppLayer : public Core::Layer
{
public:
    AppLayer() = default;
    // virtual void init() override;
    // virtual void update() override;
    void open_project();
    Core::Event<const ProjectMetadata&> on_project_update; 
    ~AppLayer() {};

private:
    std::filesystem::path m_project_root;
    ProjectMetadata m_cur_proj_data;
};