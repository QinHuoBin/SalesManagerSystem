#pragma once
#include"Utils.h"

using namespace std;
// 命令模块
class CommandModule
{
public:
	// 打印帮助界面，并等待用户输入
	// 返回-1：退出程序
	virtual int enter_module()=0;

	const string module_name;// 储存模块名称

protected:
	LoginInfo* login_info;// 指明当前模块的使用用户，便于在内部判断是否具有使用权限,
	sqlite3* db;// 本模块使用的数据库
};