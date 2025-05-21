#pragma once

using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::ComponentModel;

namespace FBStoreA
{
	public ref class MainWnd : Window, INotifyPropertyChanged
	{
	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;
	protected:
		void OnPropertyChanged(String^ propertyName);

		DependencyObject^ _root = nullptr;
		MenuItem^ _login_mi = nullptr;
	private:	
		String^ propLogin = "Login";
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

	public:
		MainWnd();
	protected:
		void Init();
	private:
		void OnOpenFile(Object^ sender, RoutedEventArgs^ e);
		void OnClose(Object^ sender, ExecutedRoutedEventArgs^ e);
		void OnLogin(Object^ sender, RoutedEventArgs^ e);

		void BindMenuItem(MenuItem^ menuItem, String^ propertyName);
	};
}
