#include "FBase.h"

#include "firebase/app.h"
#include "firebase/auth.h"

#include <iostream>
#include <thread>
#include <chrono>
#include "firebase/future.h"

using namespace firebase;
using namespace firebase::auth;

namespace FBStoreA
{
	bool FBase::LoginAnon()
	{
		if (_fbApp)
		{
			delete _fbApp;
			std::cout << "Delete Firebase App." << std::endl;
		}
		_fbApp = App::Create();
		if (!_fbApp)
		{
			std::cout << "Failed to initialize Firebase App." << std::endl;
			return false;
		}

		if (_fbAuth)
		{
			delete _fbAuth;
			std::cout << "Delete Firebase Auth." << std::endl;
		}
		_fbAuth = Auth::GetAuth(_fbApp);
		if (!_fbAuth)
		{
			std::cout << "Failed to initialize Firebase Auth." << std::endl;
			return false;
		}

		Future<AuthResult> future = _fbAuth->SignInAnonymously();
		while (future.status() == firebase::kFutureStatusPending)
		{
			Sleep(100);
		}

		if (future.status() == firebase::kFutureStatusComplete)
		{
			if (future.error() == firebase::auth::kAuthErrorNone)
			{
				std::cout << "Anonymous sign-in successful!" << std::endl;
				const firebase::auth::User user = future.result()->user;
				if (user.is_valid())
					std::cout << "User ID: " << user.uid() << std::endl;
				else {
					std::cout << "User Invalid error: " << future.error_message() << std::endl;
					return false;
				}
			}
			else {
				std::cout << "Error signing in anonymously: " << future.error_message() << std::endl;
				return false;
			}
		}
		else {
			std::cout << "Sign-in operation did not complete." << std::endl;
			return false;
		}
		return true;
	}

	FBase::~FBase()
	{
		if (_fbAuth)	
			_fbAuth->SignOut();		
		delete _fbApp;
	}
	
	bool FBase::IsLogin()
	{
		_userName = "";
		if (!_fbAuth)
			return false;
		const User user = _fbAuth->current_user();
		if (!user.is_valid())
			return false;
		_userName = user.display_name();
	}

	void FBase::Logout()
	{
		if (_fbAuth)
		{
			_fbAuth->SignOut();
			_userName = "";
		}
	}
}

