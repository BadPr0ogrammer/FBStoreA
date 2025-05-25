#include "LoginDlg.h"
#include "MainWnd.h"
#include "Utilities.h"

using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Windows::Data;
using namespace System::Windows::Media;
using namespace System::Windows::Markup;
using namespace System::Windows::Media::Imaging;
using namespace System::Security::Cryptography;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace Newtonsoft::Json;

namespace FBStoreA
{
	LoginDlg::LoginDlg()
	{
		FileStream^ fs = gcnew FileStream("LoginDlg.xaml", FileMode::Open);
		_root = (DependencyObject^) XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		Height = tmp->Height;
		Width = tmp->Width;
		WindowStartupLocation = tmp->WindowStartupLocation;
		Content = tmp->Content;
		DataContext = this;

		_anon_btn = (Button^) tmp->FindName("Button_Anon");
		_google_btn = (Button^) tmp->FindName("Button_Google");
		_output_tb = (TextBox^)tmp->FindName("TextBox_Output");

        _google_btn->Click += gcnew System::Windows::RoutedEventHandler(this, &LoginDlg::OnLogin);

        ImageFileToControl("btn_google_sign-in.png", _google_btn);
	}

	void LoginDlg::OnLogin(Object^ sender, RoutedEventArgs^ e)
	{
        DialogResult = Login() ? true : false;
	}
		
	void LoginDlg::OnCancel(Object^ sender, RoutedEventArgs^ e)
	{
		DialogResult = false;
	}

    void LoginDlg::OnAnon(Object^ sender, RoutedEventArgs^ e)
    {
        _anon = true;
        DialogResult = true;
    }

    void LoginDlg::Output(String^ str)
    {
        _output_tb->Text = _output_tb->Text + str + Environment::NewLine;
        Console::WriteLine(str);
    }

    bool LoginDlg::Login()
    {
        String^ state = randomDataBase64url(32);
        String^ code_verifier = randomDataBase64url(32);
        String^ code_challenge = base64urlencodeNoPadding(sha256(code_verifier));
        String^ code_challenge_method = "S256";
        String^ redirectURI = String::Format("http://{0}:{1}/", IPAddress::Loopback, GetRandomUnusedPort());
        Output("redirect URI: " + redirectURI);
        HttpListener^ http = gcnew HttpListener();
        http->Prefixes->Add(redirectURI);
        Output("Listening..");
        http->Start();
        String^ authorizationRequest =
            String::Format("{0}?response_type=code&scope=openid%20profile&redirect_uri={1}&client_id={2}&state={3}&code_challenge={4}&code_challenge_method={5}",
                authorizationEndpoint,
                Uri::EscapeDataString(redirectURI),
                clientID,
                state,
                code_challenge,
                code_challenge_method);
        Process::Start(authorizationRequest);
        HttpListenerContext^ context;
        try
        {
            context = http->GetContextAsync()->GetAwaiter().GetResult();
            this->Activate();
            auto response = context->Response;
            String^ responseString = String::Format("<html><head><meta http-equiv='refresh' content='10;url=https://google.com'></head><body>Please return to the app.</body></html>");
            auto buffer = Encoding::UTF8->GetBytes(responseString);
            response->ContentLength64 = buffer->Length;
            auto responseOutput = response->OutputStream;
            responseOutput->Write(buffer, 0, buffer->Length);
            responseOutput->Close();
            http->Stop();
            Output("HTTP server stopped.");
        }
        catch (Exception^ ex)
        {
            Console::WriteLine("Error: {0}", ex->Message);
            return false;
        }
        if (context->Request->QueryString->Get("error") != nullptr)
        {
            Output(String::Format("OAuth authorization error: {0}.", context->Request->QueryString->Get("error")));
            return false;
        }
        if (context->Request->QueryString->Get("code") == nullptr
            || context->Request->QueryString->Get("state") == nullptr)
        {
            Output("Malformed authorization response. " + context->Request->QueryString);
            return false;
        }
        auto code = context->Request->QueryString->Get("code");
        auto incoming_state = context->Request->QueryString->Get("state");
        // Compares the receieved state to the expected value, to ensure that
        // this app made the request which resulted in authorization.
        if (incoming_state != state)
        {
            Output(String::Format("Received request with invalid state ({0})", incoming_state));
            return false;
        }
        Output("Authorization code: " + code);

        Dictionary<String^, String^>^ tokenEndpointD;
        if (tokenEndpointD = GetTokens(code, code_verifier, redirectURI))
        {
            _tokenEndpoint = tokenEndpointD;
            return true;
        }
        return false;
    }

    Dictionary<String^, String^>^ LoginDlg::GetTokens(String^ code, String^ code_verifier, String^ redirectURI)
    {
        Output("Exchanging code for tokens...");
        String^ tokenRequestBody = String::Format("code={0}&redirect_uri={1}&client_id={2}&code_verifier={3}&client_secret={4}&scope=&grant_type=authorization_code",
            code,
            System::Uri::EscapeDataString(redirectURI),
            clientID,
            code_verifier,
            clientSecret);
        HttpWebRequest^ tokenRequest = (HttpWebRequest ^) WebRequest::Create(tokenEndpoint);
        tokenRequest->Method = "POST";
        tokenRequest->ContentType = "application/x-www-form-urlencoded";
        tokenRequest->Accept = "Accept=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
        auto _byteVersion = Encoding::ASCII->GetBytes(tokenRequestBody);
        tokenRequest->ContentLength = _byteVersion->Length;
        Stream^ stream = tokenRequest->GetRequestStream();
        stream->Write(_byteVersion, 0, _byteVersion->Length);
        stream->Close();

        try
        {
            WebResponse^ tokenResponse = tokenRequest->GetResponse();
            {
                StreamReader^ reader = gcnew StreamReader(tokenResponse->GetResponseStream());
                String^ responseText = reader->ReadToEnd();
                Output(responseText);
                Dictionary<String^, String^>^ tokenEndpointD = JsonConvert::DeserializeObject<Dictionary<String^,String^>^>(responseText);
                String^ access_token = tokenEndpointD["access_token"];
                UserinfoCall(access_token);
                return tokenEndpointD;
            }
        }
        catch (WebException^ ex)
        {
            if (ex->Status == WebExceptionStatus::ProtocolError)
            {
                auto response = (HttpWebResponse^) ex->Response;
                if (response != nullptr)
                {
                    Output(String::Format("HTTP: {0}", response->StatusCode));
                    {
                        StreamReader^ reader = gcnew StreamReader(response->GetResponseStream());
                        String^ responseText = reader->ReadToEnd();
                        Output(responseText);
                    }
                }
            }
        }
        return nullptr;
    }

    void LoginDlg::UserinfoCall(String^ access_token)
    {
        Output("Making API Call to Userinfo...");
        HttpWebRequest^ userinfoRequest = (HttpWebRequest^) WebRequest::Create(userInfoEndpoint);
        userinfoRequest->Method = "GET";
        userinfoRequest->Headers->Add(String::Format("Authorization: Bearer {0}", access_token));
        userinfoRequest->ContentType = "application/x-www-form-urlencoded";
        userinfoRequest->Accept = "Accept=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
        WebResponse^ userinfoResponse = userinfoRequest->GetResponse();
        StreamReader^ userinfoResponseReader = gcnew StreamReader(userinfoResponse->GetResponseStream());
        String^ userinfoResponseText = userinfoResponseReader->ReadToEnd();
        Output(userinfoResponseText);
    }
}