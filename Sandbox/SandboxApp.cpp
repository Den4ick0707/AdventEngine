#include <Application.hpp>

class Sandbox : public AdventEngine::Core::Application
{
public:
    Sandbox() {}
    ~Sandbox() {}

    void Update(float dt) override {
    }

    void Render() override {
    }
};

std::unique_ptr<AdventEngine::Core::Application> AdventEngine::Core::CreateApplication()
{
    return std::make_unique<Sandbox>();
}