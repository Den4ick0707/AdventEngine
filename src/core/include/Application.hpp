#ifndef __ADVENTENGINE_CORE_APPLICATION_H__
#define __ADVENTENGINE_CORE_APPLICATION_H__

namespace AdventEngine::Core
{
class Application
{

  public:
    Application();
    virtual ~Application();
    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    void Run();
    virtual void Update(float dt) = 0;
    virtual void Render()         = 0;

    void Quit()
    {
        Running = false;
    }

  private:
    static Application* s_Instance;
    bool Running = true;
};

Application* CreateApplication();
}
#endif
