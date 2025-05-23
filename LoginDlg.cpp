#include "LoginDlg.h"
#include "MainWnd.h"

using namespace System::Windows::Data;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Windows::Media;
using namespace System::Windows::Markup;
using namespace System::Collections;

namespace FBStoreA
{
	LoginDlg::LoginDlg(MainWnd^ wnd)
	{
		_mainWnd = wnd;

		FileStream^ fs = gcnew FileStream("LoginDlg.xaml", FileMode::Open);
		_root = (DependencyObject^) XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		Height = tmp->Height;
		Width = tmp->Width;
		WindowStartupLocation = tmp->WindowStartupLocation;
		Content = tmp->Content;
		DataContext = this;

		Button^ login_btn = (Button^) tmp->FindName("Button_Login");
		login_btn->Click += gcnew RoutedEventHandler(this, &LoginDlg::OnLogin);

		Button^ cancel_btn = (Button^) tmp->FindName("Button_Cancel");
		cancel_btn->Click += gcnew RoutedEventHandler(this, &LoginDlg::OnCancel);

		_username_tb = (TextBox^) tmp->FindName("TextBox_Username");
		_username_tb->Text = _mainWnd->_username;
		_password_tb = (TextBox^) tmp->FindName("PasswordBox_Password");
		_password_tb->Text = _mainWnd->_password;

		_anonymously_chb = (CheckBox^)tmp->FindName("CheckBox_Anon");
		_anonymously_chb->IsChecked = _mainWnd->_anonymously;
	}

	void LoginDlg::OnLogin(Object^ sender, RoutedEventArgs^ e)
	{
		_mainWnd->_username = _username_tb->Text;
		_mainWnd->_password = _password_tb->Text;
		_mainWnd->_anonymously = _anonymously_chb->IsChecked.HasValue && _anonymously_chb->IsChecked.Value;
		DialogResult = true;
	}

	void LoginDlg::OnCancel(Object^ sender, RoutedEventArgs^ e)
	{
		DialogResult = false;
	}
}