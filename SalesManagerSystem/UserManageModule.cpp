#include "UserManageModule.h"

UserManageModule::UserManageModule(LoginInfo* info, sqlite3* db)
{
	this->login_info = info;
	this->db = db;
}

void UserManageModule::modify_my_password()
{
	printf("�޸��ҵ�����\n");
	string new_password = ask_question_get_str("�����������룺");
	string new_password_repeat = ask_question_get_str("���ٴ�����������:");
	if (new_password != new_password_repeat) {
		printf("�������벻һ�£�\n");
		return;
	}
	if (new_password.empty()) {
		printf("���벻��Ϊ��\n");
	}

	int result = db_modify_user(login_info->user.username, login_info->user.username, new_password, login_info->user.user_type);
	if (!result) {
		printf("�޸�����ʧ�ܣ�\n");
	}
	else {
		printf("�޸�����ɹ�\n");
	}
}

/*
* �����û�
*/
void UserManageModule::add_user()
{
	if (!check_admin(login_info))
		return;
	printf("�����û�\n");

	string username = ask_question_get_str("�������û�����");
	if (username.empty()) {
		printf("�û�������Ϊ��\n");
		return;
	}

	if (db_find_user(username)) {
		printf("�û��Ѵ���\n");
	}

	string password = ask_question_get_str("���������룺");
	string password_repeat = ask_question_get_str("���ٴ���������:");
	if (password != password_repeat) {
		printf("�������벻һ�£�\n");
		return;
	}
	if (password.empty()) {
		printf("���벻��Ϊ��\n");
		return;
	}

	printf("��ѡ���˻����ͣ�\n");
	for (auto type_id : UserType_All) {
		printf("\t%d. %s\n", type_id, get_usertype_str(type_id).c_str());
	}
	printf("��ѡ��");
	int user_type = get_num();
	if (get_usertype_str(UserType(user_type)) == ERROR_USERTYPE_STR) {
		printf("�����ڴ��˻����ͣ�\n");
		return;
	}

	int result = db_add_user(username, password, UserType(user_type));
	if (result) {
		printf("�����û��ɹ�\n");
	}
	else {
		printf("�����û�ʧ�ܣ�\n");
	}


}

void UserManageModule::remove_user()
{
	if (!check_admin(login_info))
		return;

	printf("ɾ���û�\n");

	string username = ask_question_get_str("�û�����");
	int result =db_remove_user(username);
	if (result) {
		printf("ɾ���û��ɹ�\n");
	}
	else {
		printf("ɾ���û�ʧ��\n");
	}
}

void UserManageModule::modify_user()
{
	if (!check_admin(login_info))
		return;

	printf("�޸��˻���Ϣ\n");

	string old_username = ask_question_get_str("Ŀ���û�����");
	if (!db_find_user(old_username)) {
		printf("�Ҳ������û���");
		return;
	}
	bool modify_self = false;
	if (old_username == login_info->user.username) {
		printf("�������޸ĵ�ǰ�˻��������޸���Ϻ����µ�¼ϵͳ�������������ϵͳ�쳣\n");
		modify_self = true;
	}

	bool keep_username = ask_yes_or_no("����ԭ�û�����");
	string new_username = login_info->user.username;
	if (!keep_username) {
		new_username = ask_question_get_str("���û�����");
		if (new_username.empty()) {
			printf("�û�������Ϊ��\n");
			return;
		}
	}

	bool keep_password = ask_yes_or_no("����ԭ���룿");
	string password = login_info->user.password;
	if (!keep_password) {
		password = ask_question_get_str("���������룺");
		string password_repeat = ask_question_get_str("���ٴ���������:");
		if (password != password_repeat) {
			printf("�������벻һ�£�\n");
			return;
		}
		if (password.empty()) {
			printf("���벻��Ϊ��\n");
		}
	}


	bool keep_usertype = ask_yes_or_no("����ԭ�˻����ͣ�");
	int user_type = int(login_info->user.user_type);
	if (!keep_usertype) {
		printf("��ѡ���˻����ͣ�\n");
		for (auto type_id : UserType_All) {
			printf("\t%d. %s\n", type_id, get_usertype_str(type_id).c_str());
		}
		printf("��ѡ��"); user_type = get_num();
		if (get_usertype_str(UserType(user_type)) == ERROR_USERTYPE_STR) {
			printf("�����ڴ��˻����ͣ�\n");
			return;
		}
	}

	if (keep_username && keep_password && keep_usertype) {
		printf("�˻���Ϣ���䣬�˳�\n");
		return;
	}

	int result = db_modify_user(old_username, new_username, password, UserType(user_type));
	if (result) {
		if (modify_self) {
			login_info->user.username = new_username;
			login_info->user.password = password;
			login_info->user.user_type = UserType(user_type);
		}
		printf("�޸��û���Ϣ�ɹ�\n");

	}
	else {
		printf("�޸��û���Ϣʧ�ܣ�\n");
	}
}

void UserManageModule::list_user()
{
	if (!check_admin(login_info))
		return;
	printf("�����г��˻���\n");
	vector<User> user_list;
	int result=db_list_user(user_list);
	if (!result) {
		printf("�г��û�ʧ�ܣ�");
		//return;
	}
	for (User& user : user_list) {
		printf("%d\t%s\t%s\t%s\n", user.id, user.username.c_str(), user.password.c_str(), get_usertype_str(user.user_type).c_str());
	}
	printf("���\n");
}




/*
* ��prepared statement��ֹsqlע��
*/
int UserManageModule::db_add_user(string username, string password, UserType type)
{

	string sql = "INSERT INTO User (id,user_type,username,password) "\
		"VALUES (NULL,?,?,?);";

	/*���¶�������https://stackoverflow.com/questions/61794729/how-does-prepared-statements-in-sqlite-c-work*/
	sqlite3_stmt* stmt; // will point to prepared stamement object
	sqlite3_prepare_v2(
		db,            // the handle to your (opened and ready) database
		sql.c_str(),    // the sql statement, utf-8 encoded
		sql.length(),   // max length of sql statement
		&stmt,          // this is an "out" parameter, the compiled statement goes here
		nullptr);       // pointer to the tail end of sql statement (when there are 
						// multiple statements inside the string; can be null)

	sqlite3_bind_int(stmt, 1, int(type));

	sqlite3_bind_text(
		stmt,             // previously compiled prepared statement object
		2,                // parameter index, 1-based
		username.c_str(),  // the data
		username.length(), // length of data
		SQLITE_STATIC);

	sqlite3_bind_text(
		stmt,             // previously compiled prepared statement object
		3,                // parameter index, 1-based
		password.c_str(),  // the data
		password.length(), // length of data
		SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error\n");
		printf("rc=%d\n", rc);
		return false;
	}
	else {
		return true;
	}

}

int UserManageModule::db_remove_user(string username)
{
	string sql = "DELETE FROM User WHERE username = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc == SQLITE_DONE) {
		return true;
	}
	else {
		fprintf(stderr, "SQL error\n");
		printf("rc=%d\n", rc);
		return false;
	}
}

int UserManageModule::db_modify_user(string old_username, string new_username, string new_password, UserType new_type)
{
	string sql = "UPDATE User "\
		"SET username = ?, password = ?, user_type = ? "\
		"WHERE username = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, new_username.c_str(), new_username.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, new_password.c_str(), new_password.length(), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, int(new_type));
	sqlite3_bind_text(stmt, 4, old_username.c_str(), old_username.length(), SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc == SQLITE_DONE) {
		return true;
	}
	else {
		fprintf(stderr, "SQL error\n");
		printf("rc=%d\n", rc);
		return false;
	}
}

int UserManageModule::db_find_user(string username)
{
	string sql = "SELECT * FROM User WHERE username = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);

	int rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return true;
	}
	else if (rc == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		return false;
	}
	else {
		fprintf(stderr, "SQL error\n");
		printf("rc=%d\n", rc);
		sqlite3_finalize(stmt);
		return false;
	}
}

int UserManageModule::db_list_user(vector<User>& user_list)
{
	string sql = "SELECT id,username,password,user_type FROM User;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
	while (1) {
		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			User user;
			user.id = sqlite3_column_int(stmt, 0);
			user.username = string((char*)sqlite3_column_blob(stmt, 1));
			user.password = string((char*)sqlite3_column_blob(stmt,2));
			user.user_type = UserType(sqlite3_column_int(stmt, 3));
			user_list.push_back(user);
		}
		else if (rc == SQLITE_DONE) {
			sqlite3_finalize(stmt);
			return true;
		}
		else {
			fprintf(stderr, "SQL error\n");
			printf("rc=%d\n", rc);
			sqlite3_finalize(stmt);
			return false;
		}
	}
	printf("�����߼�������Ӧִ�е�����\n");
	return false;
}

#define MODIFY_MY_PASSWORD 1
#define ADD_USER 2
#define REMOVE_USER 3
#define MODIFY_USER 4
#define LIST_USER 5
#define UMM_EXIT 6
int UserManageModule::enter_module()
{
	while (1) {
		printf("��ѡ����\n");
		printf("%d. �޸��ҵ�����\n", MODIFY_MY_PASSWORD);
		printf("%d. ����û�����Ȩ��\n", ADD_USER);
		printf("%d. ɾ���û�����Ȩ��\n", REMOVE_USER);
		printf("%d. �޸��û���Ϣ����Ȩ��\n", MODIFY_USER);
		printf("%d. �г������û�����Ȩ��\n", LIST_USER);
		printf("%d. �˳���ģ��\n", UMM_EXIT);

		int cmd = get_num();
		switch (cmd) {
		case MODIFY_MY_PASSWORD:modify_my_password(); break;
		case ADD_USER:add_user(); break;
		case REMOVE_USER:remove_user(); break;
		case MODIFY_USER:modify_user(); break;
		case LIST_USER:list_user(); break;
		case UMM_EXIT:return -1; break;
		default:
			printf("����������������ԣ�\n");
			continue;
		}
	}
	return 0;
}
