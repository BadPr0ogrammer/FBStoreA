#pragma once

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
#include "Project.h"

namespace FBStoreA
{
	ref class MainWnd;

	public ref class LoginDlg : Window
	{
		MainWnd^ _mainWnd =	nullptr;

		Button^ _anon_btn =	nullptr;
		Button^ _google_btn = nullptr;
		TextBox^ _output_tb = nullptr;

		DependencyObject^ _root = nullptr;

		// client configuration
		String^ clientSecret = PROJECT_SECRET;
		String^ tokenEndpoint = "https://www.googleapis.com/oauth2/v4/token";
		String^ userInfoEndpoint = "https://www.googleapis.com/oauth2/v3/userinfo";
		String^ clientID = PROJECT_CLIENT_ID;
		String^ authorizationEndpoint = "https://accounts.google.com/o/oauth2/v2/auth";
	public:
		Dictionary<String^, String^>^ _tokenEndpoint = nullptr;
		bool _anon = false;

		LoginDlg();
	private:
		void OnLogin(Object^ sender, RoutedEventArgs^ e);
		void OnCancel(Object^ sender, RoutedEventArgs^ e);
		void OnAnon(Object^ sender, RoutedEventArgs^ e);

		bool Login();
		void Output(String^ str);
		Dictionary<String^, String^>^ GetTokens(String^ code, String^ code_verifier, String^ redirectURI);
		void UserinfoCall(String^ access_token);
	};
}