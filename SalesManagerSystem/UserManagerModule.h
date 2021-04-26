#pragma once
#include"CommandModule.h"
#include"sqlite_orm.h"
class UserManageModule :
	public CommandModule
{
public:

	enum UserType {
		Salesperson, Admin
	};

	struct User {
		int id;
		UserType user_type;
		string username;
		string password;// Ӧ�ü���
	};

	enum AuthenticationState
	{
		LOGIN_SUCCESS, USERNAME_NOT_EXISTS, WRONG_PASSWORD
	};

	struct LoginInfo
	{
		AuthenticationState auth_state;
		UserType user_type;
	};

	void authenticate(string username, string password, LoginInfo& info);

private:
	void add_user(string username, string password, UserType type);



	// ͨ�� CommandModule �̳�
	virtual int enter_command() override;

};

