#pragma once
#include "../../Time/CETimestamp.h"

extern class Application* CreateApplication();

class Application
{
public:
    virtual ~Application();
    
    virtual bool Init();

    virtual void Tick(CETimestamp Deltatime);

    virtual bool Release();

    class CEScene* Scene = nullptr;
};

extern Application* GApplication;