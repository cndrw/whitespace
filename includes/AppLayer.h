#pragma once

#include <filesystem>
#include <vector>

#include "Layer.h"
#include "Event.h"

typedef struct {
    uint16_t active_scene_idx;
    std::string project_name;
    std::vector<std::string> scene_list;
} ProjectMetadata;

class AppLayer : public Core::Layer
{
public:
    AppLayer() = default;
    // virtual void init() override;
    // virtual void update() override;
    ~AppLayer() {};

    void open_project();
    void load_scene(const std::string& scene_name);
    std::string get_cur_scene_name() const;
    Core::Event<const ProjectMetadata&> on_project_update; 

private:
    std::filesystem::path m_project_root;
    ProjectMetadata m_cur_proj_data;
};