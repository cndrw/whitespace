#pragma once

#include "Layer.h"
#include "Event.h"
#include "Inspector.h"
#include "AssetExplorer.h"

class UILayer : public Core::Layer
{
public:
    UILayer();
    virtual void init() override;
    virtual void update() override;
    virtual void render() override;
    virtual ~UILayer();
    
    AssetExplorer* get_asset_explorer() { return &m_asset_explorer; } 

private:
    Inspector m_inspector;
    AssetExplorer m_asset_explorer;

};