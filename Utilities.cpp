#include "Utilities.h"

using namespace System;
using namespace System::Security::Cryptography;
using namespace System::Text;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;
using namespace System::Security::Cryptography;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Collections::Generic;
using namespace System::Windows::Media::Imaging;

using namespace Newtonsoft::Json;

namespace FBStoreA
{
	std::string ConvertToStdString(String^ managedString)
	{
		using namespace Runtime::InteropServices;
		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(managedString)).ToPointer();
		std::string nativeString(chars);
		Marshal::FreeHGlobal(System::IntPtr((void*)chars));
		return nativeString;
	}

    // Base64url no-padding encodes the given input buffer.
    String^ base64urlencodeNoPadding(array<Byte>^ buffer)
    {
        String^ base64 = Convert::ToBase64String(buffer);

        // Converts base64 to base64url.
        base64 = base64->Replace("+", "-");
        base64 = base64->Replace("/", "_");
        // Strips padding.
        base64 = base64->Replace("=", "");

        return base64;
    }

    // Returns URI-safe data with a given input length.
    // Input length (nb. output will be longer)
    String^ randomDataBase64url(unsigned int length)
    {
        RNGCryptoServiceProvider^ rng = gcnew RNGCryptoServiceProvider();
        array<Byte>^ bytes = gcnew array<Byte>(length);
        rng->GetBytes(bytes);
        return base64urlencodeNoPadding(bytes);
    }

    // Returns the SHA256 hash of the input string.
    array<Byte>^ sha256(String^ inputStirng)
    {
        array<Byte>^ bytes = Encoding::ASCII->GetBytes(inputStirng);
        SHA256Managed^ sha256 = gcnew SHA256Managed();
        return sha256->ComputeHash(bytes);
    }

    // ref http://stackoverflow.com/a/3978040
    int GetRandomUnusedPort()
    {
        TcpListener^ listener = gcnew TcpListener(IPAddress::Loopback, 0);
        listener->Start();
        int port = ((IPEndPoint^)listener->LocalEndpoint)->Port;
        listener->Stop();
        return port;
    }

    void ImageFileToControl(String^ file, ContentControl^ obj)
    {
        Image^ image = gcnew Image();
        BitmapImage^ bitmap = gcnew BitmapImage();
        bitmap->BeginInit();
        bitmap->UriSource = gcnew Uri(file, UriKind::Relative);
        bitmap->EndInit();
        image->Source = bitmap;
        obj->Content = image;
    }
}