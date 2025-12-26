#pragma once

namespace Core
{
    class Layer
    {
    public:
        virtual void init() {};
        virtual void update() {};
        virtual void render() {};
        virtual ~Layer() = default;
        void transition_to();
            // Application::get().push_layer<Layer>();
    };

}