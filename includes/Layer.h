#pragma once

class Layer
{
public:
    virtual ~Layer() = default;
    virtual void update() {};
    virtual void render() {};
    void transition_to();
        // Application::get().push_layer<Layer>();
};
