#include "UserManageModule.h"

UserManageModule::UserManageModule(LoginInfo* info, sqlite3* db)
{
	this->login_info = info;
	this->db = db;
}

void UserManageModule::modify_my_password()
{
	printf("修改我的密码\n");
	string new_password = ask_question_get_str("请输入新密码：");
	string new_password_repeat = ask_question_get_str("请再次输入新密码:");
	if (new_password != new_password_repeat) {
		printf("两次密码不一致！\n");
		return;
	}
	if (new_password.empty()) {
		printf("密码不得为空\n");
	}

	int result = db_modify_user(login_info->user.username, login_info->user.username, new_password, login_info->user.user_type);
	if (!result) {
		printf("修改密码失败！\n");
	}
	else {
		printf("修改密码成功\n");
	}
}

/*
* 创建用户
*/
void UserManageModule::add_user()
{
	if (!check_admin(login_info))
		return;
	printf("创建用户\n");

	string username = ask_question_get_str("请输入用户名：");
	if (username.empty()) {
		printf("用户名不得为空\n");
		return;
	}

	if (db_find_user(username)) {
		printf("用户已存在\n");
	}

	string password = ask_question_get_str("请输入密码：");
	string password_repeat = ask_question_get_str("请再次输入密码:");
	if (password != password_repeat) {
		printf("两次密码不一致！\n");
		return;
	}
	if (password.empty()) {
		printf("密码不得为空\n");
		return;
	}

	printf("请选择账户类型：\n");
	for (auto type_id : UserType_All) {
		printf("\t%d. %s\n", type_id, get_usertype_str(type_id).c_str());
	}
	printf("请选择：");
	int user_type = get_num();
	if (get_usertype_str(UserType(user_type)) == ERROR_USERTYPE_STR) {
		printf("不存在此账户类型！\n");
		return;
	}

	int result = db_add_user(username, password, UserType(user_type));
	if (result) {
		printf("创建用户成功\n");
	}
	else {
		printf("创建用户失败！\n");
	}


}

void UserManageModule::remove_user()
{
	if (!check_admin(login_info))
		return;

	printf("删除用户\n");

	string username = ask_question_get_str("用户名：");
	int result =db_remove_user(username);
	if (result) {
		printf("删除用户成功\n");
	}
	else {
		printf("删除用户失败\n");
	}
}

void UserManageModule::modify_user()
{
	if (!check_admin(login_info))
		return;

	printf("修改账户信息\n");

	string old_username = ask_question_get_str("目标用户名：");
	if (!db_find_user(old_username)) {
		printf("找不到此用户！");
		return;
	}
	bool modify_self = false;
	if (old_username == login_info->user.username) {
		printf("你正在修改当前账户，请在修改完毕后重新登录系统，否则可能引起系统异常\n");
		modify_self = true;
	}

	bool keep_username = ask_yes_or_no("保持原用户名？");
	string new_username = login_info->user.username;
	if (!keep_username) {
		new_username = ask_question_get_str("新用户名：");
		if (new_username.empty()) {
			printf("用户名不得为空\n");
			return;
		}
	}

	bool keep_password = ask_yes_or_no("保持原密码？");
	string password = login_info->user.password;
	if (!keep_password) {
		password = ask_question_get_str("请输入密码：");
		string password_repeat = ask_question_get_str("请再次输入密码:");
		if (password != password_repeat) {
			printf("两次密码不一致！\n");
			return;
		}
		if (password.empty()) {
			printf("密码不得为空\n");
		}
	}


	bool keep_usertype = ask_yes_or_no("保持原账户类型？");
	int user_type = int(login_info->user.user_type);
	if (!keep_usertype) {
		printf("请选择账户类型：\n");
		for (auto type_id : UserType_All) {
			printf("\t%d. %s\n", type_id, get_usertype_str(type_id).c_str());
		}
		printf("请选择："); user_type = get_num();
		if (get_usertype_str(UserType(user_type)) == ERROR_USERTYPE_STR) {
			printf("不存在此账户类型！\n");
			return;
		}
	}

	if (keep_username && keep_password && keep_usertype) {
		printf("账户信息不变，退出\n");
		return;
	}

	int result = db_modify_user(old_username, new_username, password, UserType(user_type));
	if (result) {
		if (modify_self) {
			login_info->user.username = new_username;
			login_info->user.password = password;
			login_info->user.user_type = UserType(user_type);
		}
		printf("修改用户信息成功\n");

	}
	else {
		printf("修改用户信息失败！\n");
	}
}

void UserManageModule::list_user()
{
	if (!check_admin(login_info))
		return;
	printf("正在列出账户：\n");
	vector<User> user_list;
	int result=db_list_user(user_list);
	if (!result) {
		printf("列出用户失败！");
		//return;
	}
	for (User& user : user_list) {
		printf("%d\t%s\t%s\t%s\n", user.id, user.username.c_str(), user.password.c_str(), get_usertype_str(user.user_type).c_str());
	}
	printf("完毕\n");
}




/*
* 用prepared statement防止sql注入
*/
int UserManageModule::db_add_user(string username, string password, UserType type)
{

	string sql = "INSERT INTO User (id,user_type,username,password) "\
		"VALUES (NULL,?,?,?);";

	/*以下段落来自https://stackoverflow.com/questions/61794729/how-does-prepared-statements-in-sqlite-c-work*/
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
	printf("程序逻辑出错：不应执行到这里\n");
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
		printf("请选择功能\n");
		printf("%d. 修改我的密码\n", MODIFY_MY_PASSWORD);
		printf("%d. 添加用户（特权）\n", ADD_USER);
		printf("%d. 删除用户（特权）\n", REMOVE_USER);
		printf("%d. 修改用户信息（特权）\n", MODIFY_USER);
		printf("%d. 列出所有用户（特权）\n", LIST_USER);
		printf("%d. 退出本模块\n", UMM_EXIT);

		int cmd = get_num();
		switch (cmd) {
		case MODIFY_MY_PASSWORD:modify_my_password(); break;
		case ADD_USER:add_user(); break;
		case REMOVE_USER:remove_user(); break;
		case MODIFY_USER:modify_user(); break;
		case LIST_USER:list_user(); break;
		case UMM_EXIT:return -1; break;
		default:
			printf("命令输入错误，请重试！\n");
			continue;
		}
	}
	return 0;
}
