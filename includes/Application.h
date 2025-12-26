#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "layer.h"

namespace Core
{
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

        template<typename TLayer>
		requires(std::derived_from<TLayer, Layer>)
		TLayer* get_layer()
		{
			for (const auto& layer : m_layers)
			{
				if (auto casted = dynamic_cast<TLayer*>(layer.get()))
					return casted;
			}
			return nullptr;
		}

    private:
        std::vector<std::unique_ptr<Layer>> m_layers;
        bool m_is_running;
    };

}