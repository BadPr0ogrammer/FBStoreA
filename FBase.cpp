#include "FBase.h"

#include "firebase/app.h"
#include "firebase/auth.h"

#include <iostream>
#include <thread>
#include <chrono>
#include "firebase/future.h"

using namespace firebase;
using namespace firebase::auth;
using namespace std;

namespace FBStoreA
{
	bool reset_app(App*& _fbApp, Auth*& _fbAuth)
	{
		if (_fbApp)
		{
			delete _fbApp;
			cout << "Delete Firebase App." << endl;
		}
		_fbApp = App::Create();
		if (!_fbApp)
		{
			cout << "Failed to initialize Firebase App." << endl;
			return false;
		}
		_fbAuth = Auth::GetAuth(_fbApp);
		if (!_fbAuth)
		{
			cout << "Failed to initialize Firebase Auth." << endl;
			return false;
		}
		return true;
	}

	bool complete_app(App* _fbApp, Auth* _fbAuth, Future<AuthResult>& future, string& _email, string& _uid)
	{
		while (future.status() == kFutureStatusPending)
		{
			Sleep(100);
		}
		if (future.status() == kFutureStatusComplete)
		{
			if (future.error() == kAuthErrorNone)
			{
				const User user = future.result()->user;
				if (user.is_valid()) {
					cout << "Sign-in successful!" << endl;
					cout << "User ID: " << user.uid() << "; Email: " << user.email() << endl;
					_email = user.email();
					_uid = user.uid();
				}
				else {
					cout << "User Invalid error: " << future.error_message() << endl;
					return false;
				}
			}
			else {
				cout << "Error signing in: " << future.error_message() << endl;
				return false;
			}
		}
		else {
			cout << "Sign-in operation did not complete." << endl;
			return false;
		}
		return true;
	}

	bool FBase::LoginAnon()
	{
		if (!reset_app(_fbApp, _fbAuth))
			return false;
		Future<AuthResult> future = _fbAuth->SignInAnonymously();
		return complete_app(_fbApp, _fbAuth, future, _email, _uid);
	}

	bool FBase::SignupWithEmail(string email, string password)
	{
		if (!reset_app(_fbApp, _fbAuth))
			return false;
		Future<AuthResult> future = _fbAuth->CreateUserWithEmailAndPassword(email.c_str(), password.c_str());
		return complete_app(_fbApp, _fbAuth, future, _email, _uid);
	}

	bool FBase::LoginWithEmail(string email, string password)
	{
		if (!reset_app(_fbApp, _fbAuth))
			return false;
		Future<AuthResult> future = _fbAuth->SignInWithEmailAndPassword(email.c_str(), password.c_str());
		return complete_app(_fbApp, _fbAuth, future, _email, _uid);
	}

	bool FBase::IsLogin()
	{
		if (!_fbAuth)
			return false;
		const User user = _fbAuth->current_user();
		if (!user.is_valid())
			return false;
		return true;
	}

	void FBase::Logout()
	{
		if (_fbAuth)
			_fbAuth->SignOut();
	}

	FBase::~FBase()
	{
		Logout();
		delete _fbApp;
	}
}

