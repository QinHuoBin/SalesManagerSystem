#pragma once
#include<string>
using namespace std;
// ����ģ��
class CommandModule
{
public:
	// ����-1���˳�����
	virtual int enter_command()=0;
};