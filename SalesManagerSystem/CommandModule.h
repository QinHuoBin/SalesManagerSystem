#pragma once
#include<string>
using namespace std;
// 命令模块
class CommandModule
{
public:
	// 返回-1：退出程序
	virtual int enter_command()=0;
};