#include "App.h"
#include "MainWnd.h"
#include "FBase.h"

using namespace System::Windows::Data;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Windows::Markup;
using namespace System::Collections;

namespace FBStoreA
{
	MainWnd::MainWnd()
	{
		Init();
	}

	void MainWnd::Init()
	{
		FileStream^ fs = gcnew FileStream(L"MainWnd.xaml", FileMode::Open);
		_root = (DependencyObject^)XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		Content = tmp->Content;
		DataContext = this;

		MenuItem^ file_mi = (MenuItem^)tmp->FindName("MenuItem_File");
		file_mi->FontSize = 16;

		MenuItem^ open_mi = (MenuItem^)tmp->FindName("MenuItem_Open");
		open_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnOpenFile);

		_login_mi = (MenuItem^)tmp->FindName("MenuItem_Login");
		_login_mi->FontSize = 16;
		_login_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnLogin);

		BindMenuItem(_login_mi, "PropLogin");

		CommandBinding^ closeBinding = gcnew CommandBinding(ApplicationCommands::Close);
		closeBinding->Executed += gcnew ExecutedRoutedEventHandler(this, &MainWnd::OnClose);
		this->CommandBindings->Add(closeBinding);
	}

	void MainWnd::OnOpenFile(Object^ sender, RoutedEventArgs^ e)
	{
	}

	void MainWnd::OnLogin(Object^ sender, RoutedEventArgs^ e)
	{
		App^ app = (App^)Application::Current;
		if (app->_fbptr->IsLogin())
		{
			app->_fbptr->Logout();
			PropLogin = "Login";
		}
		else
		{
			if (app->_fbptr->LoginAnon())
				PropLogin = "Logout";
		}
	}

	void MainWnd::OnClose(Object^ sender, ExecutedRoutedEventArgs^ e) 
	{
		App^ app = (App^)Application::Current;
		delete app->_fbptr;

		this->Close(); 
	}

	void MainWnd::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	void MainWnd::BindMenuItem(MenuItem^ menuItem, String^ propertyName)
	{
		Binding^ binding = gcnew Binding(propertyName);
		binding->Source = this;
		//binding->Mode = BindingMode::TwoWay; 
		menuItem->SetBinding(MenuItem::HeaderProperty, binding);
	}

}