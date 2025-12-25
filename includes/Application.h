#pragma once

#include <vector>
#include <memory>

#include "layer.h"

class Application
{
public:
    Application();
    static Application& get();
    void run();
    void stop();

    template<typename TLayer>
    requires(std::derived_from<TLayer, Layer>)
    void push_layer()
    {
        m_layers.push_back(std::make_unique<TLayer>());
    }


private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    bool m_is_running;
};
