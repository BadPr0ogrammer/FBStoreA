#pragma once

using namespace System;
using namespace System::Windows;

#include "MainWnd.h"
#include "FBase.h"

namespace FBStoreA
{
    ref class App : Application
    {
    public:
        MainWnd^ _wnd = nullptr;
        FBase* _fbptr = nullptr;

        App();
    protected:
        virtual void OnStartup(StartupEventArgs^ e) override;
    };
}