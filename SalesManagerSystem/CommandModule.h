#pragma once
#include"Utils.h"

using namespace std;
// ����ģ��
class CommandModule
{
public:
	// ��ӡ�������棬���ȴ��û�����
	// ����-1���˳�����
	virtual int enter_module()=0;

	const string module_name;// ����ģ������

protected:
	LoginInfo* login_info;// ָ����ǰģ���ʹ���û����������ڲ��ж��Ƿ����ʹ��Ȩ��,
	sqlite3* db;// ��ģ��ʹ�õ����ݿ�
};