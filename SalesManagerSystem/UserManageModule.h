#pragma once
#include"CommandModule.h"
class UserManageModule :
	public CommandModule
{
public:
	const string module_name = "用户管理";
	sqlite3* db;
	UserManageModule(LoginInfo *info, sqlite3* db);

//private:
	// 人机交互接口
	void modify_my_password();
	void add_user();
	void remove_user();
	void modify_user();
	void list_user();

	// 数据库操作
	int db_add_user(string username, string password, UserType type);
	int db_remove_user(string username);
	int db_modify_user(string old_username,string new_username, string new_password, UserType new_type);
	int db_find_user(string username);
	int db_list_user(vector<User>&);


	// 通过 CommandModule 继承
	virtual int enter_module() override;

};

