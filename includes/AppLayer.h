#pragma once

#include <filesystem>
#include <vector>

#include "Layer.h"
#include "Event.h"

typedef struct SceneData {
    std::string scene_name;
    std::filesystem::path save_dir;

    inline std::filesystem::path full_path() const { return std::filesystem::path(save_dir).append(scene_name); }

    bool operator==(const SceneData& second) const
    {
        return full_path() == second.full_path();
    }
} SceneData;

typedef struct {
    uint16_t active_scene_idx;
    std::string project_name;
    SceneData last_opened_scene;
    std::vector<SceneData> scene_list;
} ProjectMetadata;

class AppLayer : public Core::Layer
{
public:
    template<typename T>
    using Result = std::pair<bool, T>;

    AppLayer() = default;
    // virtual void init() override;
    // virtual void update() override;
    ~AppLayer() {};

    Result<std::string> open_project(std::filesystem::path project_file);
    void load_scene(const std::string& scene_name);
    SceneData get_cur_scene() const;
    Core::Event<const ProjectMetadata&> on_project_update; 

private:
    std::filesystem::path m_project_root;
    ProjectMetadata m_cur_proj_data;
};