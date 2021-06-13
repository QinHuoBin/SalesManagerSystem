/*
* 销售管理系统
* 作者：自动化类2012覃活彬，学号20205299
*/

using namespace std;

#include "SalesManagerSystem.h"

#include"LoginModule.h"
#include"SaleGoodsModule.h"
#include"GoodsManageModule.h"
#include"UserManageModule.h"
#include"Utils.h"

SalesManagerSystem::SalesManagerSystem()
{
	prepare_database();
	open_database(DEFAULT_DATABASE_NAME, &db);
}




#define LOGIN_MODULE 0
#define SALE_GOODS_MODULE 1
#define USER_MANAGE_MODULE 2
#define GOODS_MANAGE_MODULE 3
void SalesManagerSystem::start()
{
	test();

	// 外层循环：初始化系统
	while (1) {
		LoginInfo login_info = require_login();
		int exit_flag = false;

		CommandModule* sale_goods = new SaleGoodsModule(&login_info, db, &GLOBAL_user_map, &GLOBAL_goods_map);
		CommandModule* user_manage = new UserManageModule(&login_info, db);
		CommandModule* goods_manage = new GoodsManageModule(&login_info, db);

		modules.insert(pair<int, CommandModule* >(SALE_GOODS_MODULE, sale_goods));
		modules.insert(pair<int, CommandModule* >(USER_MANAGE_MODULE, user_manage));
		modules.insert(pair<int, CommandModule* >(GOODS_MANAGE_MODULE, goods_manage));



		int exit_cmd = modules.size() + 1;

		// 内层循环：模块选择
		while (1) {
			// 加载所有商品
			refresh_global_map((UserManageModule*)user_manage, (GoodsManageModule*)goods_manage);

			printf("请选择模块\n");
			printf("%d.销售商品模块\n", SALE_GOODS_MODULE);
			printf("%d.用户管理模块\n", USER_MANAGE_MODULE);
			printf("%d.商品管理模块\n", GOODS_MANAGE_MODULE);
			printf("%d.登出\n", exit_cmd);
			int cmd = get_num();

			auto result = modules.find(cmd);
			if (cmd == exit_cmd) {
				break;

			}
			if (result == modules.end()) {
				printf("找不到对应模块！\n");
			}
			else {
				printf("\n");
				int exec_return = result->second->enter_module();
				//if (exec_return == -2)
				//	break;
			}
			printf("\n");
		}
		printf("正在登出...\n");
		// 释放模块对象
		for (auto& pair : modules) {
			delete (pair.second);
		}
		modules.clear();
		printf("\n");

	}
}


void SalesManagerSystem::exit_sys()
{
	sqlite3_close(db);
	exit(0);
}



#define CMD_SYS_LOGIN 1
#define CMD_SYS_EXIT 2
LoginInfo SalesManagerSystem::require_login()
{

	while (1) {
		printf("欢迎使用销售管理系统，请选择操作：\n");
		printf("1. 登录\n");
		printf("2. 退出\n");

		LoginModule login = (db);

		int cmd = get_num();
		if (cmd == CMD_SYS_LOGIN)
		{
			bool flag_for_login_success = false;
			LoginInfo info;
			while (1) {
				info = login.enter_module();
				if (info.auth_state != AuthenticationState::LOGIN_SUCCESS) {
					bool retry = ask_yes_or_no("登录失败，是否重试？");
					if (retry) {
						continue;
					}
					else {
						break;
					}
				}
				else {
					flag_for_login_success = true;
					printf("\n");
					break;
				}
			}
			if (flag_for_login_success)
				return info;
		}
		else if (cmd == CMD_SYS_EXIT) {
			printf("正在退出系统...\n");
			exit_sys();
			return *(new LoginInfo);
		}
		else {
			printf("找不到命令，请重新输入.\n");
			continue;
		}
		printf("\n");
	}

}

// 准备数据库，包括其中的表
void SalesManagerSystem::prepare_database()
{
	// 判断文件是否存在，若存在，则退出
	FILE* file = fopen(DEFAULT_DATABASE_NAME, "r");
	if (file != NULL) {
		fclose(file);
		return;
	}
	else {
		printf("正在准备数据库\n");
	}

	sqlite3* db = NULL;
	int rc;
	char* zErrMsg = 0;
	open_database(DEFAULT_DATABASE_NAME, &db);

	// 为UserManageModule创建表
	/*
		struct User {
		int id;
		int user_type;
		string username;
		string password;// 应该加密
	};
	*/
	string sql = "CREATE TABLE User ("\
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"\
		"user_type INTEGER NOT NULL,"\
		"username CHAR(50) NOT NULL UNIQUE,"\
		"password CHAR(50) NOT NULL);";

	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		//fprintf(stdout, "Table created successfully\n");
	}

	//struct Goods {
	//	int id;
	//	string goods_name;
	//	float price;
	//	int quantity;
	//};
	sql = "CREATE TABLE Goods ("\
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"\
		"goods_name CHAR(50) NOT NULL UNIQUE,"\
		"price REAL NOT NULL,"\
		"quantity INTEGER NOT NULL);";
	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	sql = "CREATE TABLE Goods_order ("\
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"\
		"time INTEGER NOT NULL,"\
		"goods_id INTEGER NOT NULL,"\
		"goods_name CHAR(50) NOT NULL,"\
		"salesperson_id INTEGER NOT NULL,"\
		"salesperson_name CHAR(50) NOT NULL,"\
		"price REAL NOT NULL,"\
		"quantity INTEGER NOT NULL);";
	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		//fprintf(stdout, "Table created successfully\n");
	}

	// 添加测试账户
	/*
	enum UserType {
		Salesperson=0, Admin=1
	};
	*/
	sql = "INSERT INTO User (id,user_type,username,password) "\
		"VALUES (NULL,1,'admin','admin');";
	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		//fprintf(stdout, "Test user create successfully\n");
	}

	// 添加测试商品
	sql = "INSERT INTO Goods (id,goods_name,price,quantity) "\
		"VALUES (NULL,'banana',1.0,200);";
	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	// 添加测试订单
	sql = "INSERT INTO Goods_order (id,time,salesperson_id,goods_id,price,quantity) "\
		"VALUES (NULL,1620005391,1,1,1.0,100);";
	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	sqlite3_close(db);
}

void SalesManagerSystem::refresh_global_map(void* user_manage, void* goods_manage)
{
	GLOBAL_goods_map.clear();
	vector<Goods> goods_list;
	((GoodsManageModule*)goods_manage)->db_list_goods(goods_list);
	for (Goods& goods : goods_list) {
		GLOBAL_goods_map.insert(pair<int, Goods>(goods.id, goods));
	}

	GLOBAL_user_map.clear();
	vector<User> user_list;
	((UserManageModule*)user_manage)->db_list_user(user_list);
	for (User& user : user_list) {
		GLOBAL_user_map.insert(pair<int, User>(user.id, user));
	}

}

void SalesManagerSystem::test() {
	//printf("进行单元测试\n");
	// 测试登录
	login_info = LoginModule(db).authenticate("admin", "admin");
	if (login_info.auth_state != AuthenticationState::LOGIN_SUCCESS) {
		printf("login fail\n");
		return;
	}

	UserManageModule* user_manage = new UserManageModule(&login_info, db);
	// 测试添加用户
	int result = user_manage->db_add_user("root", "root", UserType::Admin);
	if (!result) {
		printf("添加用户失败\n");
	}

	result = user_manage->db_find_user("root");
	if (!result) {
		printf("查找用户失败！");
	}

	result = user_manage->db_modify_user("root", "root_modify", "new_password", UserType::Salesperson);
	if (!result) {
		printf("修改用户信息失败！");
	}

	vector<User> user_list;
	result = user_manage->db_list_user(user_list);
	if (!result) {
		printf("列出用户失败！");
	}

	result = user_manage->db_remove_user("root_modify");
	if (!result) {
		printf("删除用户失败！");
	}
	delete user_manage;

	GoodsManageModule* goods_manage = new GoodsManageModule(&login_info, db);
	result = goods_manage->db_add_goods("apple", 8888, 6666);
	if (!result) {
		printf("添加商品失败！");
	}

	result = goods_manage->db_modify_goods("apple", "apple_modify", 1, 2);
	if (!result) {
		printf("修改商品信息失败！");
	}

	Goods goods;
	result = goods_manage->db_find_goods("apple_modify", &goods);
	if (!result || goods.goods_name != "apple_modify") {
		printf("查找商品失败！");
	}

	vector<Goods> goods_list;
	result = goods_manage->db_list_goods(goods_list);
	if (!result || goods_list.size() < 1) {
		printf("列出商品失败！");
	}

	result = goods_manage->db_remove_goods("apple_modify");
	if (!result) {
		printf("删除商品失败！");
	}

	delete goods_manage;

	SaleGoodsModule* sale_goods = new SaleGoodsModule(&login_info, db, &GLOBAL_user_map, &GLOBAL_goods_map);
	result = sale_goods->db_record_order(114514, 1, "banana", 1, "admin", 3, 3);

	if (!result) {
		printf("添加销售记录失败！");
	}

	result = sale_goods->db_decrease_goods_quantity(0, 1);
	if (!result) {
		printf("减少商品数量失败！");
	}

	vector<Order> all_orders;
	result = sale_goods->db_enquire_all_orders(all_orders);
	if (!result || all_orders.size() < 1) {
		printf("减少商品数量失败！");
	}
	delete sale_goods;

	//printf("测试全部通过\n");
}