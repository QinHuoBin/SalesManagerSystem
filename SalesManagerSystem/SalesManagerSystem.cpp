// SalesManagerSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// Sqlite ORM
//  https://github.com/fnc12/sqlite_orm

#include <iostream>
using namespace std;

#include"sqlite_orm.h"
using namespace sqlite_orm;

#include "SalesManagerSystem.h"

#include"GoodsManageModule.h"
#include"UserManagerModule.h"



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

SalesManagerSystem::SalesManagerSystem()
{
	prepare_database();
}

decltype(make_storage("db.sqlite",
	make_table("users",
		make_column("id", &UserManageModule::User::id, autoincrement(), primary_key()),
		make_column("user_type", &UserManageModule::User::user_type),
		make_column("username", &UserManageModule::User::username),
		make_column("password", &UserManageModule::User::password)),
	make_table("goods",
		make_column("id", &GoodsManageModule::Goods::id, autoincrement(), primary_key()),
		make_column("goods_name", &GoodsManageModule::Goods::goods_name),
		make_column("price", &GoodsManageModule::Goods::price),
		make_column("quantity", &GoodsManageModule::Goods::quantity))
))* storage;

// 准备数据库，包括其中的表
void SalesManagerSystem::prepare_database()
{
	storage = &make_storage("db.sqlite",
		make_table("users",
			make_column("id", &UserManageModule::User::id, autoincrement(), primary_key()),
			make_column("user_type", &UserManageModule::User::user_type),
			make_column("username", &UserManageModule::User::username),
			make_column("password", &UserManageModule::User::password)),
		make_table("goods",
			make_column("id", &GoodsManageModule::Goods::id, autoincrement(), primary_key()),
			make_column("goods_name", &GoodsManageModule::Goods::goods_name),
			make_column("price", &GoodsManageModule::Goods::price),
			make_column("quantity", &GoodsManageModule::Goods::quantity))
	);
	//storage.insert
	storage->sync_schema();// 创建数据库文件
}
