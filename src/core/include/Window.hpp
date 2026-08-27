#ifndef __ADVENTENGINE_CORE_WINDOW_HPP__
#define __ADVENTENGINE_CORE_WINDOW_HPP__


namespace AdventEngine::Core
{
struct WindowProps
{
    std::string title = "AdventEngine";
    std::uint32_t width = 1280;
    std::uint32_t height = 720;
};

class Window
{
public:
    virtual ~Window() = default;

    virtual void PollEvents() = 0;

    [[nodiscard]]
    virtual bool ShouldClose() const = 0;

    [[nodiscard]]
    virtual std::uint32_t GetWidth() const = 0;

    [[nodiscard]]
    virtual std::uint32_t GetHeight() const = 0;

    static std::unique_ptr<Window> Create(
        const WindowProps& props = {}
    );
};
}

#endif