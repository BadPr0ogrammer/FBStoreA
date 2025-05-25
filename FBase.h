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
		std::string _user;
		std::string _email;

		FBase() {}

		bool LoginAnon();
		bool IsLogin();
		void Logout();
		bool Login(std::string id_token, std::string access_token);

		~FBase();
	};
}

