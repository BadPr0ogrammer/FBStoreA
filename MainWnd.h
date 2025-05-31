#pragma once

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::Windows::Media::Media3D;
using namespace System::Windows::Media;
using namespace System::ComponentModel;
using namespace System::Collections;

using namespace HelixToolkit;
using namespace HelixToolkit::Geometry;
using namespace HelixToolkit::Wpf;
#ifdef FBXSDK_SHARED
namespace fbxsdk
{
	class FbxNode;
}
#else
struct ufbx_node;
#endif

namespace FBStoreA
{
	class FbxLoader;

	public ref class MainWnd : Window, INotifyPropertyChanged
	{
	protected:
		void OnPropertyChanged(String^ propertyName);
	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

		DependencyObject^ _root = nullptr;

		MenuItem^ _login_mi = nullptr;
		MenuItem^ _user_mi = nullptr;
		TextBlock^ _transform_sb_tb = nullptr;

		HelixViewport3D^ _viewport = nullptr;
		Model3DGroup^ _modelGroup = nullptr;
		ModelVisual3D^ _modelVisual = nullptr;

		FbxLoader* _fbxLoader = nullptr;

	private:	
		String^ propLogin = "Login";
		String^ propUser = "";
		String^ propTransform = "";
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
		property String^ PropUser {
			String^ get() { return propUser; }
			void set(String^ value) {
				if (propUser != value) {
					propUser = value;
					OnPropertyChanged("PropUser");
				}
			}
		}

	public:
		MainWnd();
		~MainWnd();
	private:
		void OnOpenFile(Object^ sender, RoutedEventArgs^ e);
		void OnClose(Object^ sender, ExecutedRoutedEventArgs^ e);
		void OnLogin(Object^ sender, RoutedEventArgs^ e);
		void OnUser(Object^ sender, RoutedEventArgs^ e);

		void BindMenuItem(MenuItem^ menuItem, String^ propertyName);
#ifdef FBXSDK_SHARED
		void BuildMeshes(fbxsdk::FbxNode* node);
#else
		void BuildMeshes(ufbx_node* node);
#endif
		void OnViewportMouseMove(Object^ sender, MouseEventArgs^ e);
	};
}
