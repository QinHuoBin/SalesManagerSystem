#pragma once
#include"CommandModule.h"
#include<vector>
#include<map>

class SalesManagerSystem
{
public:
	SalesManagerSystem();
	void test();
	void start();// 进入循环命令处理流程
	map<int,CommandModule*> modules;
	sqlite3* db;
	LoginInfo login_info;
	void exit_sys();// 释放资源并退出
private :
		void prepare_database();
		// UserManageModule* user_manage, GoodsManageModule* goods_manage
		void refresh_global_map(void* user_manage, void* goods_manage);
		LoginInfo require_login();
};

