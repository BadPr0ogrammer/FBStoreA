using namespace System;

#include "App.h"
#include "FBase.h"

[System::STAThreadAttribute]
int main(array<System::String^>^ args)
{
    FBStoreA::FBase* fb = new FBStoreA::FBase();

    FBStoreA::App^ app = gcnew FBStoreA::App();
    app->_fbptr = fb;
    app->Run();

    delete fb;
    return 0;
}
