#pragma once

#include <filesystem>

#include "Layer.h"

class AppLayer : public Core::Layer
{
public:
    AppLayer() = default;
    // virtual void init() override;
    // virtual void update() override;
    void open_project();
    ~AppLayer() {};

private:
    std::filesystem::path m_project_root;
};