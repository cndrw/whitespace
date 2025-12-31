#pragma once

#include "Layer.h"

class ProjectLayer : public Core::Layer
{
public:
    ProjectLayer();
    virtual void init() override;
    virtual void update() override;
    virtual void render() override;
    virtual ~ProjectLayer();
};