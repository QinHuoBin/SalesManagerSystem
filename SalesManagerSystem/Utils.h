#pragma once

#include"sqlite3.h"
#include<cstdio>
#include <stdio.h>
#include<string>
#include<vector>
#include<map>
using namespace std;

#define DEFAULT_DATABASE_NAME "sales.db"
// 打开数据库
inline void open_database(string filename, sqlite3** db) {
    char* zErrMsg = 0;
    int rc;

    rc = sqlite3_open(filename.c_str(), db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        exit(-1);
    }
    else {
        //fprintf(stderr, "Opened database successfully\n");
    }
}

// 依然有bug
inline void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 从stdin读取一个字母，若为Y或y，返回true；若为N或n，返回false；
// 若为回车，则默认
// 否则不断重试
// 注意：会刷新输入流
// todo: 把输入流改为无缓冲的
// todo: 把注释和规范搞好来
inline bool ask_yes_or_no(string question,bool default_yes=true)
{
    while (1)
    {
        if (default_yes)
            printf("%s [Y/n]: ", question.c_str());
        else
            printf("%s [y/N]: ", question.c_str());
        char ch;
        scanf("%c", &ch);

        if (ch == 'Y' || ch == 'y')
            return true;
        else if (ch == 'N' || ch == 'n')
            return false;
        else if (ch == '\n')
            return default_yes == true;
        else
            clear_stdin();
    }
    
}

inline string ask_question_get_str(string question) {

    printf("%s",question.c_str());
    string get_str();
    return get_str();
}

// 从stdin读取一个整数，并将其返回（顺便刷新输入流）
inline int get_num() {
    int num;

    scanf("%d", &num);
    clear_stdin();
    return num;
}

inline float get_float() {
    float num;

    scanf("%f", &num);
    clear_stdin();
    return num;
}

// 从stdin读取一个字符串，并将其返回（顺便刷新输入流）
inline string get_str() {
    char str[128]="";

    scanf("%s", str);
    clear_stdin();
    return string(str);
}

enum class UserType {
	Salesperson = 0, Admin = 1
};

static const UserType UserType_All[] = { UserType::Salesperson ,UserType::Admin };

#define ERROR_USERTYPE_STR "不存在的账户类型"
inline string get_usertype_str(UserType type) {
    switch (type) {
    case UserType::Salesperson:return "销售员"; break;
    case UserType::Admin:return "系统管理员"; break;
    default:
        printf("系统内部错误：不存在%d对应的用户类型！", type);
        return ERROR_USERTYPE_STR;
    }
}




struct User {
	int id=-1;
	UserType user_type=UserType::Salesperson;
	string username;
	string password;// 应该加密
};

struct Goods {
    int id;
    string goods_name;
    float price;
    int quantity;
};


inline void print_goods_header()
{
    printf("id\tgoods_name\tprice\tquantity\n");
}

inline void print_goods_row(Goods& goods)
{
    printf("%d\t%s\t%f\t%d\n", goods.id, goods.goods_name.c_str(), goods.price, goods.quantity);
}

// 只被SaleGoodsModule使用（避免发生耦合）
// 注意这不是线程安全的
// 会在主模块和SaleGoodsModule::record_order里更新
extern map<int, Goods> GLOBAL_goods_map;
extern map<int, User> GLOBAL_user_map;

// 订单结构体
struct Order {
    int id = -1;
    time_t time;

    int salesperson_id;
    string salesperson_name;
    int goods_id;
    string goods_name;
    float price;
    int num;
};

enum class AuthenticationState
{
	LOGIN_SUCCESS, USERNAME_NOT_EXISTS, WRONG_PASSWORD
};

struct LoginInfo
{
	AuthenticationState auth_state=AuthenticationState::USERNAME_NOT_EXISTS;
	User user;// 只有当auth_state为LOGIN_SUCCESS时，user才不为空
};

/*
* 是管理员则返回true
* todo: 添加审计
*/
inline bool check_admin(LoginInfo* login_info) {
    if (login_info->user.user_type == UserType::Admin) {
        return true;
    }
    else {
        printf("该功能需要系统管理员操作！\n");
        return false;
    }
}


