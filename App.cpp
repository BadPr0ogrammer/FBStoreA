#include "App.h"

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Windows::Markup;

namespace FBStoreA
{
	App::App()
	{
	}

	void App::OnStartup(StartupEventArgs^ e)
	{
		FileStream^ fs = gcnew FileStream("App.xaml", FileMode::Open);
		Resources = (ResourceDictionary^)XamlReader::Load(fs);
		fs->Close();

		//Resources[]
		//_paletteHelper = gcnew PaletteHelper();
		//_paletteHelper->SetTheme(_theme);

		_wnd = gcnew MainWnd();
		_wnd->Show();
	}
}