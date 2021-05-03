#include "SaleGoodsModule.h"

#include<time.h>
#include<algorithm>

SaleGoodsModule::SaleGoodsModule(LoginInfo* info, sqlite3* db, map<int, User>* _user_map, map<int, Goods>* _goods_map)
{
	login_info = info;
	this->db = db;
	user_map = _user_map;
	goods_map = _goods_map;
}

void SaleGoodsModule::add_goods_to_list()
{
	printf("请选择要添加的商品\n");

	// 打印所有商品
	print_goods_header();
	for (auto& goods_pair : *goods_map) {
		Goods& goods = goods_pair.second;
		print_goods_row(goods);
	}

	int goods_id = get_num();
	auto result = goods_map->find(goods_id);
	if (result == goods_map->end()) {
		printf("商品不存在！");
		return;
	}

	Goods& goods = result->second;


	printf("请输入你要添加的数量\n");
	int quantity = get_num();
	if (quantity > goods.quantity) {
		printf("抱歉，没有那么多这种商品！\n");
		return;
	}

	Order order;
	order.goods_id = goods_id;
	order.price = goods.price;
	order.num = quantity;
	order.salesperson_id = login_info->user.id;

	int loc = find_order_in_list(order_list, goods_id);
	if (loc == -1) {
		order_list.push_back(order);
	}
	else {
		order_list[loc].num += quantity;
	}

	printf("添加完毕\n");
}

void SaleGoodsModule::remove_goods_in_list()
{
	printf("这是你的购物清单：\n");
	print_order_list(order_list);
	printf("请选择你要删除的序号：");
	int order_id = get_num();
	if (order_id >= order_list.size()) {
		printf("没有此序号！\n");
		return;
	}

	auto del = order_list.begin();
	order_list.erase(del + order_id);

	printf("删除完毕\n");
}

void SaleGoodsModule::modify_goods_in_list()
{
	printf("这是你的购物清单：\n");
	print_order_list(order_list);

	printf("请选择你要修改的序号：");
	int order_id = get_num();
	if (order_id >= order_list.size()) {
		printf("没有此序号！\n");
		return;
	}

	Order& order = order_list[order_id];
	Goods& goods = (*goods_map)[order.goods_id];


	printf("此商品现有%d份，请输入你要购买的数量：", goods.quantity);
	int new_quantity = get_num();
	if (new_quantity > goods.quantity) {
		printf("抱歉，没有那么多这种商品！\n");
		return;
	}
}

void SaleGoodsModule::give_up_list()
{
	bool clear = ask_yes_or_no("真的要清除购物车吗？", false);
	if (clear) {
		order_list.clear();
		printf("已清空\n");
	}
	else {
		printf("不清空\n");
	}
}

void SaleGoodsModule::charge_by_list()
{
	printf("这是你的购物清单：\n");
	print_order_list(order_list);

	bool charge = ask_yes_or_no("是否结账？");
	if (!charge) {
		printf("退出\n");
		return;
	}

	printf("请输入你给的钱：");
	float money = get_float();

	float should_pay = 0;
	for (auto& order : order_list) {
		should_pay += order.price * order.num;
	}

	if (money < should_pay) {
		printf("你给的钱太少了！\n");
		return;
	}

	for (Order& order : order_list) {
		record_order(order);
	}

	printf("找零：%lf\n", double(money) - should_pay);
	printf("欢迎下次光临！\n");

	order_list.clear();

}

void SaleGoodsModule::print_order_list(vector<Order>& order_list)
{
	printf("序号\t商品名\t单价\t数量\t价格\n");
	int i = 0;
	float sum_money = 0;
	for (auto& order : order_list) {
		Goods& goods = goods_map->find(order.goods_id)->second;
		sum_money += order.num * order.price;
		printf("%d\t%s\t%f\t%d\t%f\n", i++, goods.goods_name.c_str(), order.price, order.num, double(order.num) * order.price);
	}
	printf("总计：%f\n", sum_money);
}

int SaleGoodsModule::find_order_in_list(vector<Order>& order_list, int goods_id) {
	for (int i = 0; i < order_list.size(); i++) {
		Order& order = order_list[i];
		if (order.goods_id == goods_id) {
			return i;
		}
	}
	return -1;
}

bool cmp(pair<int, float>& T1, pair<int, float>& T2) {
	if (T1.second > T2.second) {
		return true;
	}
	else {
		return false;
	}
}

// 打印报表
void SaleGoodsModule::print_account()
{
	if (!check_admin(login_info))
		return;

	// 两个方面：销售额最多的销售员和商品

	vector<Order> all_orders;
	db_enquire_all_orders(all_orders);

	map<int, float> salesperson_rank_map;
	map<int, float> goods_rank_map;
	for (auto& order : all_orders) {
		salesperson_rank_map[order.salesperson_id] += order.num * order.price;
		goods_rank_map[order.goods_id] += order.num * order.price;
	}

	vector<pair<int, float>> salesperson_rank;
	for (auto& the_pair : salesperson_rank_map) {
		salesperson_rank.push_back(the_pair);
	}
	vector<pair<int, float>> goods_rank;
	for (auto& the_pair : goods_rank_map) {
		goods_rank.push_back(the_pair);
	}

	// 排序
	sort(salesperson_rank.begin(), salesperson_rank.end(), cmp);
	sort(goods_rank.begin(), goods_rank.end(), cmp);

	// 打印前5
	printf("销售员排名：\n");
	printf("排名\t名称\t总金额\n");
	for (int i = 0; i < 4 && i < salesperson_rank.size(); i++) {
		User& user = user_map->find(salesperson_rank[i].first)->second;
		printf("%d. %s\t%f\n", i, user.username.c_str(), salesperson_rank[i].second);
	}
	printf("----------------------\n");
	printf("商品销售排名：\n");
	printf("排名\t商品名\t总金额\n");
	for (int i = 0; i < 4 && i < goods_rank.size(); i++) {
		Goods& goods = goods_map->find(goods_rank[i].first)->second;
		printf("%d. %s\t%f\n", i, goods.goods_name.c_str(), goods_rank[i].second);
	}
}

void SaleGoodsModule::record_order(Order& order)
{
	time_t time_now = time(0);
	// 以下过程不能失败，否则系统将不一致
	// todo: 1.增加quantity数量检测，若没有足够数量商品，则返回失败，要求重新购买（线程安全）
	bool a = db_record_order(time_now, order.goods_id, order.salesperson_id, order.price, order.num);
	bool b = db_decrease_goods_quantity(order.goods_id, order.num);
	goods_map->find(order.goods_id)->second.quantity -= order.num;

	if (!a && b) {
		printf("系统内部操作失败！db_record_order: %d, db_decrease_goods_quantity: %d\n", a, b);
	}
}


int SaleGoodsModule::db_record_order(time_t time_now, int goods_id, int salesperson_id, float price, int quantity)
{
	string sql = "INSERT INTO Goods_order (id,time,goods_id,salesperson_id,price,quantity) "\
		"VALUES (NULL,?,?,?,?,?);";

	sqlite3_stmt* stmt;
	int a = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	a = sqlite3_bind_int(stmt, 1, time_now);
	a = sqlite3_bind_int(stmt, 2, goods_id);
	a = sqlite3_bind_int(stmt, 3, salesperson_id);
	a = sqlite3_bind_double(stmt, 4, price);
	a = sqlite3_bind_int(stmt, 5, quantity);

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

int SaleGoodsModule::db_decrease_goods_quantity(int goods_id, int num)
{
	string sql = "UPDATE Goods "\
		"SET quantity = quantity - ? "\
		"WHERE id = ?;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);


	sqlite3_bind_int(stmt, 1, num);
	sqlite3_bind_int(stmt, 2, goods_id);

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

int SaleGoodsModule::db_enquire_all_orders(vector<Order>& order_list)
{
	string sql = "SELECT id,time,goods_id,salesperson_id,price,quantity FROM Goods_order;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
	while (1) {
		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			Order order;
			order.id = sqlite3_column_int(stmt, 0);
			order.time = sqlite3_column_int(stmt, 1);
			order.goods_id = sqlite3_column_int(stmt, 2);
			order.salesperson_id = sqlite3_column_int(stmt, 3);
			order.price = sqlite3_column_double(stmt, 4);
			order.num = sqlite3_column_int(stmt, 5);
			order_list.push_back(order);
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


#define ADD_GOODS_TO_LIST 1
#define REMOVE_GOODS_IN_LIST 2
#define MODIFY_GOODS_IN_LIST 3
#define GIVE_UP_LIST 4
#define CHARGE_BY_LIST 5
#define PRINT_ACCOUNT 6
#define SGM_EXIT 7
int SaleGoodsModule::enter_module()
{
	while (1) {
		printf("请选择功能\n");
		printf("%d. 增添商品到购物清单\n", ADD_GOODS_TO_LIST);
		printf("%d. 删除购物清单的一个商品\n", REMOVE_GOODS_IN_LIST);
		printf("%d. 修改商品数量\n", MODIFY_GOODS_IN_LIST);
		printf("%d. 清空购物清单\n", GIVE_UP_LIST);
		printf("%d. 结账\n", CHARGE_BY_LIST);
		printf("%d. 打印销售报表（特权）\n", PRINT_ACCOUNT);
		printf("%d. 退出本模块\n", SGM_EXIT);

		int cmd = get_num();
		switch (cmd) {
		case ADD_GOODS_TO_LIST:add_goods_to_list(); break;
		case REMOVE_GOODS_IN_LIST:remove_goods_in_list(); break;
		case MODIFY_GOODS_IN_LIST:modify_goods_in_list(); break;
		case GIVE_UP_LIST:give_up_list(); break;
		case CHARGE_BY_LIST:charge_by_list(); break;
		case PRINT_ACCOUNT:print_account(); break;
		case SGM_EXIT:return -1; break;
		default:
			printf("命令输入错误，请重试！\n");
			continue;
		}
	}
	return 0;
}


