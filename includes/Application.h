#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "layer.h"

#include "AssetManager.h"

namespace Core
{
    class Application
    {
    public:
        Application();
        static Application& get();
        void run();
        void stop();

        template<typename TLayer, typename... Args>
        requires(std::derived_from<TLayer, Layer>)
        void push_layer(Args&&... args)
        {
            m_layers.push_back(std::make_unique<TLayer>(std::forward<Args>(args)...));
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

        AssetManager* get_asset_manager();

    private:
        std::vector<std::unique_ptr<Layer>> m_layers;
        AssetManager m_asset_manager;
        bool m_is_running;
    };

}