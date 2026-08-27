#include "../include/Application.hpp"

int main()
{
    auto application = AdventEngine::Core::CreateApplication();
    application->Run();

    return 0;
}