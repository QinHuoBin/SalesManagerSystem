#pragma once

#include"Utils.h"


// 此模块用于登录，不参与callback，仅能被主模块的require_login调用
class LoginModule
{
public:
	LoginModule(sqlite3* db) :db(db) {};
	// 此方法不保证用户登录成功，失败登录也返回LoginInfo，由主模块决定是否再次登录
	// 原因：让主模块进行释放资源的工作
	LoginInfo enter_module() ;

//private:
	sqlite3* db;
	LoginInfo authenticate(string username, string password);
};


