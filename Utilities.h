#pragma once

#include <string>
using namespace System;
using namespace System::Windows::Controls;

namespace FBStoreA
{
	std::string ConvertToStdString(System::String^ managedString);

    String^ base64urlencodeNoPadding(array<Byte>^ buffer);
    String^ randomDataBase64url(unsigned int length);
    array<Byte>^ sha256(String^ inputStirng);

    int GetRandomUnusedPort();

    void ImageFileToControl(String^ file, ContentControl^ obj);
}