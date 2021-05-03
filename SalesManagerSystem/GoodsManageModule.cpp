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
	printf("添加商品\n");

	string goods_name = ask_question_get_str("请输入商品名：");
	if (goods_name.empty()) {
		printf("商品名不得为空\n");
		return;
	}

	if (db_find_goods(goods_name)) {
		printf("商品已存在\n");
		return;
	}

	printf("请输入价格：");
	float price = get_float();
	if (price < 0) {
		printf("商品价格不得低于0！");
		return;
	}

	printf("请输入上架数量：");
	int quantity = get_num();
	if (quantity < 0) {
		printf("上架数量不得低于0！\n");
		return;
	}

	int result = db_add_goods(goods_name, price, quantity);
	if (result) {
		printf("创建商品成功\n");
	}
	else {
		printf("创建商品失败！\n");
	}

}

void GoodsManageModule::remove_goods()
{
	if (!check_admin(login_info))
		return;

	printf("删除商品\n");

	string goods_name = ask_question_get_str("商品名：");
	int result = db_remove_goods(goods_name);
	if (result) {
		printf("删除商品成功\n");
	}
	else {
		printf("删除商品失败\n");
	}
}

void GoodsManageModule::modify_goods()
{
	if (!check_admin(login_info))
		return;

	printf("修改商品信息\n");

	Goods the_goods;
	string old_goods_name = ask_question_get_str("目标商品名：");
	if (!db_find_goods(old_goods_name, &the_goods)) {
		printf("找不到此商品！\n");
		return;
	}

	print_goods_header();
	print_goods_row(the_goods);

	bool keep_goods_name = ask_yes_or_no("保持原商品名？");
	string new_goods_name = the_goods.goods_name;
	if (!keep_goods_name) {
		new_goods_name = ask_question_get_str("新商品名：");
		if (new_goods_name.empty()) {
			printf("商品名不得为空\n");
			return;
		}
	}

	bool keep_price = ask_yes_or_no("保持原价？");
	float new_price = the_goods.price;
	if (!keep_price) {
		printf("新价格：");
		new_price = get_float();
		if (new_price < 0) {
			printf("价格不得低于0！\n");
			return;
		}
	}

	bool keep_quantity = ask_yes_or_no("保持原数量？");
	int new_quantity = the_goods.quantity;
	if (!keep_quantity) {
		printf("新数量：");
		keep_quantity = get_num();
		if (new_quantity < 0) {
			printf("上架数量不得低于0！\n");
			return;
		}
	}

	if (keep_goods_name && keep_price && keep_quantity) {
		printf("商品信息不变，退出\n");
		return;
	}
	int result = db_modify_goods(old_goods_name, new_goods_name, new_price, new_quantity);
	if (result) {
		printf("修改商品信息成功\n");
	}
	else {
		printf("修改商品信息失败\n");
	}
}

void GoodsManageModule::list_goods()
{
	if (!check_admin(login_info))
		return;
	printf("正在列出商品：\n");

	vector<Goods> goods_list;
	int result = db_list_goods(goods_list);
	if (!result) {
		printf("列出商品失败！\n");
		//return;
	}

	print_goods_header();
	for (Goods& goods : goods_list) {
		print_goods_row(goods);
	}
	printf("完毕\n");
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
	printf("程序逻辑出错：不应执行到这里\n");
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
		printf("请选择功能\n");
		printf("%d. 添加商品（特权）\n", ADD_GOODS);
		printf("%d. 删除商品（特权）\n", REMOVE_GOODS);
		printf("%d. 列出商品（特权）\n", LIST_GOODS);
		printf("%d. 修改商品信息（特权）\n", MODIFY_GOODS);
		printf("%d. 退出本模块\n", GMM_EXIT);

		int cmd = get_num();
		switch (cmd) {
		case ADD_GOODS:add_goods(); break;
		case REMOVE_GOODS:remove_goods(); break;
		case MODIFY_GOODS:modify_goods(); break;
		case LIST_GOODS:list_goods(); break;
		case GMM_EXIT:return -1; break;
		default:
			printf("命令输入错误，请重试！\n");
			continue;
		}
	}
	return 0;
}

