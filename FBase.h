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
		std::string _email;
		std::string _uid;

		FBase() {}

		bool LoginAnon();
		bool IsLogin();
		void Logout();
		bool SignupWithEmail(std::string email, std::string password);
		bool LoginWithEmail(std::string email, std::string password);

		~FBase();
	};
}

