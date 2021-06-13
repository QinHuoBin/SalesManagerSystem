
#include "LoginModule.h"

LoginInfo LoginModule::enter_module()
{
	printf("请输入用户名：");
	string username = get_str();
	printf("请输入密码：");
	string password = get_str();
	LoginInfo info = authenticate(username, password);

	if (info.auth_state == AuthenticationState::USERNAME_NOT_EXISTS) {
		printf("用户名不存在！\n");
	}
	else if (info.auth_state == AuthenticationState::WRONG_PASSWORD) {
		printf("密码错误！\n");
	}
	else if (info.auth_state == AuthenticationState::LOGIN_SUCCESS) {
		printf("登录成功！\n");
	}

	return info;
}

LoginInfo LoginModule::authenticate(string username, string password)
{
	//info.user.password = "erased";
	//if (username == "admin" && password == "admin") {
	//	LoginInfo info;
	//	info.auth_state = AuthenticationState::LOGIN_SUCCESS;
	//	
	//	User user;
	//	user.id = 0;
	//	user.username = username;
	//	user.user_type = UserType::Admin;

	//	info.user = user;
	//	return info;
	//}
	string sql = "SELECT id,user_type,password FROM User WHERE username = ?";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, username.c_str(), username.length(), SQLITE_STATIC);

	int rc = sqlite3_step(stmt);
	
	User logged_user;
	logged_user.username = username;
	logged_user.password = "erased";

	LoginInfo login_info;// 注意，一定要先修改完logged_user，再给login_info赋值
	
	if (rc == SQLITE_ROW) {// 找到用户
		string cur_password=string((char*)sqlite3_column_blob(stmt, 2));
		if (cur_password == password) {
			logged_user.id = sqlite3_column_int(stmt, 0);
			logged_user.user_type = UserType(sqlite3_column_int(stmt, 1));

			login_info.auth_state = AuthenticationState::LOGIN_SUCCESS;
			login_info.user = logged_user;
			
			sqlite3_finalize(stmt);
			return login_info;
		}
		else {
			login_info.auth_state = AuthenticationState::WRONG_PASSWORD;
			login_info.user = logged_user;

			sqlite3_finalize(stmt);
			return login_info;
		}
	}
	else if (rc == SQLITE_DONE) {// 未找到用户

		
		login_info.auth_state = AuthenticationState::USERNAME_NOT_EXISTS;
		login_info.user = logged_user;

		sqlite3_finalize(stmt);
		return login_info;
	}
	else {
		fprintf(stderr, "SQL error\n");
		printf("rc=%d\n", rc);
		sqlite3_finalize(stmt);
		return login_info;
	}
}