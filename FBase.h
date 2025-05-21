#pragma once

#include <string>

namespace firebase 
{
	class App;
	namespace auth
	{
		class Auth;
	}
}

namespace FBStoreA
{
	class FBase
	{
	public:
		firebase::App* _fbApp = nullptr;
		firebase::auth::Auth* _fbAuth = nullptr;
		std::string _userName;

		FBase() {}
		bool LoginAnon();
		bool IsLogin();
		void Logout();

		~FBase();
	};
}

