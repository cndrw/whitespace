#pragma once

#include "Layer.h"
#include "Event.h"
#include "Inspector.h"

class UILayer : public Core::Layer
{
public:
    UILayer();
    virtual void init() override;
    virtual void update() override;
    virtual void render() override;
    virtual ~UILayer();

private:
    void update_inspector(Vector2 pos);

private:
    Inspector m_inspector;
};