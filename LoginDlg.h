#pragma once

using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::ComponentModel;

namespace FBStoreA
{
	ref class MainWnd;

	public ref class LoginDlg : Window
	{
		MainWnd^ _mainWnd = nullptr;
		TextBox^ _username_tb = nullptr;
		TextBox^ _password_tb = nullptr;
		CheckBox^ _anonymously_chb = nullptr;
	protected:
		DependencyObject^ _root = nullptr;
	public:
		LoginDlg(MainWnd^ wnd);
	private:
		void OnLogin(Object^ sender, RoutedEventArgs^ e);
		void OnCancel(Object^ sender, RoutedEventArgs^ e);
	};
}