#pragma once

namespace Core
{
    class Layer
    {
    public:
        virtual void init() {}
        virtual bool on_click() { return false; }
        virtual void update() {}
        virtual void render() {}
        virtual ~Layer() = default;
        void transition_to();
            // Application::get().push_layer<Layer>();
    };

}