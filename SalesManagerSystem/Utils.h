#pragma once

#include"sqlite3.h"
#include<cstdio>
#include <stdio.h>
#include<string>
#include<vector>
#include<map>
using namespace std;

#define DEFAULT_DATABASE_NAME "sales.db"
// �����ݿ�
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

// ��Ȼ��bug
inline void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ��stdin��ȡһ����ĸ����ΪY��y������true����ΪN��n������false��
// ��Ϊ�س�����Ĭ��
// ���򲻶�����
// ע�⣺��ˢ��������
// todo: ����������Ϊ�޻����
// todo: ��ע�ͺ͹淶�����
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

// ��stdin��ȡһ�������������䷵�أ�˳��ˢ����������
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

// ��stdin��ȡһ���ַ����������䷵�أ�˳��ˢ����������
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

#define ERROR_USERTYPE_STR "�����ڵ��˻�����"
inline string get_usertype_str(UserType type) {
    switch (type) {
    case UserType::Salesperson:return "����Ա"; break;
    case UserType::Admin:return "ϵͳ����Ա"; break;
    default:
        printf("ϵͳ�ڲ����󣺲�����%d��Ӧ���û����ͣ�", type);
        return ERROR_USERTYPE_STR;
    }
}




struct User {
	int id=-1;
	UserType user_type=UserType::Salesperson;
	string username;
	string password;// Ӧ�ü���
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

// ֻ��SaleGoodsModuleʹ�ã����ⷢ����ϣ�
// ע���ⲻ���̰߳�ȫ��
// ������ģ���SaleGoodsModule::record_order�����
extern map<int, Goods> GLOBAL_goods_map;
extern map<int, User> GLOBAL_user_map;

// �����ṹ��
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
	User user;// ֻ�е�auth_stateΪLOGIN_SUCCESSʱ��user�Ų�Ϊ��
};

/*
* �ǹ���Ա�򷵻�true
* todo: ������
*/
inline bool check_admin(LoginInfo* login_info) {
    if (login_info->user.user_type == UserType::Admin) {
        return true;
    }
    else {
        printf("�ù�����Ҫϵͳ����Ա������\n");
        return false;
    }
}


