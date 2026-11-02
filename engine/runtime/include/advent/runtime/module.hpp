#pragma once

namespace advent::runtime
{
// A piece of engine or game functionality driven by the Application loop.
// Modules are initialized in the order they were added and shut down in reverse order.
class Module
{
public:
    Module()          = default;
    virtual ~Module() = default;

    Module(const Module&)            = delete;
    Module& operator=(const Module&) = delete;
    Module(Module&&)                 = delete;
    Module& operator=(Module&&)      = delete;

    virtual void on_init() {}
    virtual void on_update(float /*delta_seconds*/) {}
    virtual void on_shutdown() {}
};
} // namespace advent::runtime
