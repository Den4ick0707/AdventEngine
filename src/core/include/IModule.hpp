#ifndef __ADVENTENGINE_CORE_IMODULE_HPP__
#define __ADVENTENGINE_CORE_IMODULE_HPP__

namespace AdventEngine::Core
{
class IModule
{
public:
    virtual ~IModule() = default;

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};
}

#endif