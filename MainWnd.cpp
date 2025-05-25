#include "App.h"
#include "MainWnd.h"
#include "LoginDlg.h"
#include "FBase.h"
#include "Utilities.h"

using namespace System::Windows;
using namespace System::Windows::Data;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Windows::Media;
using namespace System::Windows::Markup;
using namespace System::Collections;

namespace FBStoreA
{
	MainWnd::MainWnd()
	{
		FileStream^ fs = gcnew FileStream("MainWnd.xaml", FileMode::Open);
		_root = (DependencyObject^)XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		WindowStartupLocation = tmp->WindowStartupLocation;
		Content = tmp->Content;
		DataContext = this;

		MenuItem^ file_mi = (MenuItem^)tmp->FindName("MenuItem_File");
		file_mi->FontSize = 20;

		MenuItem^ open_mi = (MenuItem^)tmp->FindName("MenuItem_Open");
		open_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnOpenFile);

		_login_mi = (MenuItem^)tmp->FindName("MenuItem_Login");
		_login_mi->FontSize = 20;
		_login_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnLogin);

		_signup_mi = (MenuItem^)tmp->FindName("MenuItem_Signup");
		_signup_mi->FontSize = 20;
		_signup_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnSignup);

		BindMenuItem(_login_mi, "PropLogin");
		BindMenuItem(_signup_mi, "PropSignup");

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
			PropSignup = "Signup";
		}
		else
		{
			LoginDlg^ loginDlg = gcnew LoginDlg();
			auto result = loginDlg->ShowDialog();
			if (result)
			{
				if (loginDlg->_anon)
				{
					if (app->_fbptr->LoginAnon())
					{
						PropLogin = "Logout";
						PropSignup = "<Anon.>";
						MessageBox::Show("Login Anonymously!", "FBStoreA");
					}
				} else if (loginDlg->_tokenEndpoint != nullptr) {
					std::string access_token = ConvertToStdString(loginDlg->_tokenEndpoint["access_token"]);
					std::string id_token = ConvertToStdString(loginDlg->_tokenEndpoint["id_token"]);
					if (app->_fbptr->Login(id_token, access_token))
					{
						PropLogin = "Logout";
						PropSignup = gcnew String(app->_fbptr->_user.c_str());
						std::string msg = "Login " + app->_fbptr->_user + " !";
						MessageBox::Show(gcnew String(msg.c_str()), "FBStoreA");
					}
				}
			}
		}
	}

	void MainWnd::OnSignup(Object^ sender, RoutedEventArgs^ e)
	{/*
		App^ app = (App^)Application::Current;
		if (app->_fbptr->IsLogin())
		{
			app->_fbptr->Logout();
			PropLogin = "Login";
			PropSignup = "Signup";
			return;
		}
		LoginDlg^ loginDlg = gcnew LoginDlg(this);
		auto result = loginDlg->ShowDialog();
		if (result)
		{
			std::string user = ConvertToStdString(_username);
			std::string pw = ConvertToStdString(_password);
			if (app->_fbptr->SignupWithEmail(user, pw))
				propSignup = gcnew String(user.c_str());
		}*/
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
		menuItem->SetBinding(MenuItem::HeaderProperty, binding);
	}
}