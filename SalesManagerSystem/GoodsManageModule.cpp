#include "GoodsManageModule.h"

GoodsManageModule::GoodsManageModule(LoginInfo* info, sqlite3* db)
{
	this->login_info = info;
	this->db = db;
}

void GoodsManageModule::add_goods()
{
	if (!check_admin(login_info))
		return;
	printf("�����Ʒ\n");

	string goods_name = ask_question_get_str("��������Ʒ����");
	if (goods_name.empty()) {
		printf("��Ʒ������Ϊ��\n");
		return;
	}

	if (db_find_goods(goods_name)) {
		printf("��Ʒ�Ѵ���\n");
		return;
	}

	printf("������۸�");
	float price = get_float();
	if (price < 0) {
		printf("��Ʒ�۸񲻵õ���0��");
		return;
	}

	printf("�������ϼ�������");
	int quantity = get_num();
	if (quantity < 0) {
		printf("�ϼ��������õ���0��\n");
		return;
	}

	int result = db_add_goods(goods_name, price, quantity);
	if (result) {
		printf("������Ʒ�ɹ�\n");
	}
	else {
		printf("������Ʒʧ�ܣ�\n");
	}

}

void GoodsManageModule::remove_goods()
{
	if (!check_admin(login_info))
		return;

	printf("ɾ����Ʒ\n");

	string goods_name = ask_question_get_str("��Ʒ����");
	int result = db_remove_goods(goods_name);
	if (result) {
		printf("ɾ����Ʒ�ɹ�\n");
	}
	else {
		printf("ɾ����Ʒʧ��\n");
	}
}

void GoodsManageModule::modify_goods()
{
	if (!check_admin(login_info))
		return;

	printf("�޸���Ʒ��Ϣ\n");

	Goods the_goods;
	string old_goods_name = ask_question_get_str("Ŀ����Ʒ����");
	if (!db_find_goods(old_goods_name, &the_goods)) {
		printf("�Ҳ�������Ʒ��\n");
		return;
	}

	print_goods_header();
	print_goods_row(the_goods);

	bool keep_goods_name = ask_yes_or_no("����ԭ��Ʒ����");
	string new_goods_name = the_goods.goods_name;
	if (!keep_goods_name) {
		new_goods_name = ask_question_get_str("����Ʒ����");
		if (new_goods_name.empty()) {
			printf("��Ʒ������Ϊ��\n");
			return;
		}
	}

	bool keep_price = ask_yes_or_no("����ԭ�ۣ�");
	float new_price = the_goods.price;
	if (!keep_price) {
		printf("�¼۸�");
		new_price = get_float();
		if (new_price < 0) {
			printf("�۸񲻵õ���0��\n");
			return;
		}
	}

	bool keep_quantity = ask_yes_or_no("����ԭ������");
	int new_quantity = the_goods.quantity;
	if (!keep_quantity) {
		printf("��������");
		keep_quantity = get_num();
		if (new_quantity < 0) {
			printf("�ϼ��������õ���0��\n");
			return;
		}
	}

	if (keep_goods_name && keep_price && keep_quantity) {
		printf("��Ʒ��Ϣ���䣬�˳�\n");
		return;
	}
	int result = db_modify_goods(old_goods_name, new_goods_name, new_price, new_quantity);
	if (result) {
		printf("�޸���Ʒ��Ϣ�ɹ�\n");
	}
	else {
		printf("�޸���Ʒ��Ϣʧ��\n");
	}
}

void GoodsManageModule::list_goods()
{
	if (!check_admin(login_info))
		return;
	printf("�����г���Ʒ��\n");

	vector<Goods> goods_list;
	int result = db_list_goods(goods_list);
	if (!result) {
		printf("�г���Ʒʧ�ܣ�\n");
		//return;
	}

	print_goods_header();
	for (Goods& goods : goods_list) {
		print_goods_row(goods);
	}
	printf("���\n");
}



int GoodsManageModule::db_add_goods(string goods_name, float price, int quantity)
{
	string sql = "INSERT INTO Goods (id,goods_name,price,quantity) "\
		"VALUES (NULL,?,?,?);";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, goods_name.c_str(), goods_name.length(), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, price);
	sqlite3_bind_int(stmt, 3, quantity);

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

int GoodsManageModule::db_remove_goods(string goods_name)
{
	string sql = "DELETE FROM Goods WHERE goods_name = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, goods_name.c_str(), goods_name.length(), SQLITE_STATIC);

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

int GoodsManageModule::db_modify_goods(string old_goods_name, string new_goods_name, float new_price, int new_quantity)
{
	string sql = "UPDATE Goods "\
		"SET goods_name = ?,price = ?,quantity = ? "\
		"WHERE goods_name = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, new_goods_name.c_str(), new_goods_name.length(), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2,new_price);
	sqlite3_bind_int(stmt, 3, new_quantity);
	sqlite3_bind_text(stmt, 4, old_goods_name.c_str(), old_goods_name.length(), SQLITE_STATIC);

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

int GoodsManageModule::db_list_goods(vector<Goods> &goods_list)
{
	string sql = "SELECT id,goods_name,price,quantity FROM Goods;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
	while (1) {
		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			Goods goods;
			goods.id = sqlite3_column_int(stmt, 0);
			goods.goods_name = string((char*)sqlite3_column_blob(stmt, 1));
			goods.price = sqlite3_column_double(stmt, 2);
			goods.quantity = sqlite3_column_int(stmt, 3);
			goods_list.push_back(goods);
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

int GoodsManageModule::db_find_goods(string goods_name, Goods* goods)
{
	string sql = "SELECT id,goods_name,price,quantity FROM Goods WHERE goods_name = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	sqlite3_bind_text(stmt, 1, goods_name.c_str(), goods_name.length(), SQLITE_STATIC);

	int rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW) {
		if (goods != NULL) {
			goods->id = sqlite3_column_int(stmt, 0);
			goods->goods_name = string((char*)sqlite3_column_blob(stmt, 1));
			goods->price = sqlite3_column_double(stmt, 2);
			goods->quantity = sqlite3_column_int(stmt, 3);
		}
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

#define ADD_GOODS 1
#define REMOVE_GOODS 2
#define LIST_GOODS 3
#define MODIFY_GOODS 4
#define GMM_EXIT 5
int GoodsManageModule::enter_module()
{
	while (1) {
		printf("��ѡ����\n");
		printf("%d. �����Ʒ����Ȩ��\n", ADD_GOODS);
		printf("%d. ɾ����Ʒ����Ȩ��\n", REMOVE_GOODS);
		printf("%d. �г���Ʒ����Ȩ��\n", LIST_GOODS);
		printf("%d. �޸���Ʒ��Ϣ����Ȩ��\n", MODIFY_GOODS);
		printf("%d. �˳���ģ��\n", GMM_EXIT);

		int cmd = get_num();
		switch (cmd) {
		case ADD_GOODS:add_goods(); break;
		case REMOVE_GOODS:remove_goods(); break;
		case MODIFY_GOODS:modify_goods(); break;
		case LIST_GOODS:list_goods(); break;
		case GMM_EXIT:return -1; break;
		default:
			printf("����������������ԣ�\n");
			continue;
		}
	}
	return 0;
}

