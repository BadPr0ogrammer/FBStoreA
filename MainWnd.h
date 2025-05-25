#pragma once

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::ComponentModel;

namespace FBStoreA
{
	public ref class MainWnd : Window, INotifyPropertyChanged
	{
	protected:
		void OnPropertyChanged(String^ propertyName);
	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

		DependencyObject^ _root = nullptr;
		MenuItem^ _login_mi = nullptr;
		MenuItem^ _signup_mi = nullptr;
	private:	
		String^ propLogin = "Login";
		String^ propSignup = "Signup";
	public:
		property String^ PropLogin {
			String^ get() {	return propLogin; }
			void set(String^ value) {
				if (propLogin != value) {
					propLogin = value;
					OnPropertyChanged("PropLogin");
				}
			}
		}
		property String^ PropSignup {
			String^ get() { return propSignup; }
			void set(String^ value) {
				if (propSignup != value) {
					propSignup = value;
					OnPropertyChanged("PropSignup");
				}
			}
		}
	public:
		MainWnd();
	private:
		void OnOpenFile(Object^ sender, RoutedEventArgs^ e);
		void OnClose(Object^ sender, ExecutedRoutedEventArgs^ e);
		void OnLogin(Object^ sender, RoutedEventArgs^ e);
		void OnSignup(Object^ sender, RoutedEventArgs^ e);

		void BindMenuItem(MenuItem^ menuItem, String^ propertyName);
	};
}
