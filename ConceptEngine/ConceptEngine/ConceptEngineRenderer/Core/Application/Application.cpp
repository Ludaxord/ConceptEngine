#include "Application.h"

#include "../../Scene/CEScene.h"

Application* GApplication;

Application::~Application()
{
    SafeDelete(Scene);
}

bool Application::Init()
{
    return true;
}

void Application::Tick(CETimestamp Deltatime)
{
    UNREFERENCED_VARIABLE(Deltatime);
}

bool Application::Release()
{
    return true;
}
