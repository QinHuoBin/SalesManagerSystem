#pragma once

#include"Utils.h"


// ��ģ�����ڵ�¼��������callback�����ܱ���ģ���require_login����
class LoginModule
{
public:
	LoginModule(sqlite3* db) :db(db) {};
	// �˷�������֤�û���¼�ɹ���ʧ�ܵ�¼Ҳ����LoginInfo������ģ������Ƿ��ٴε�¼
	// ԭ������ģ������ͷ���Դ�Ĺ���
	LoginInfo enter_module() ;

//private:
	sqlite3* db;
	LoginInfo authenticate(string username, string password);
};


