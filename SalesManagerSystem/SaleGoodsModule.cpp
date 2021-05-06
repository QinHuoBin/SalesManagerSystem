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
	printf("��ѡ��Ҫ��ӵ���Ʒ\n");

	// ��ӡ������Ʒ
	print_goods_header();
	for (auto& goods_pair : *goods_map) {
		Goods& goods = goods_pair.second;
		print_goods_row(goods);
	}

	int goods_id = get_num();
	auto result = goods_map->find(goods_id);
	if (result == goods_map->end()) {
		printf("��Ʒ�����ڣ�");
		return;
	}

	Goods& goods = result->second;


	printf("��������Ҫ��ӵ�����\n");
	int quantity = get_num();
	if (quantity > goods.quantity) {
		printf("��Ǹ��û����ô��������Ʒ��\n");
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

	printf("������\n");
}

void SaleGoodsModule::remove_goods_in_list()
{
	printf("������Ĺ����嵥��\n");
	print_order_list(order_list);
	printf("��ѡ����Ҫɾ������ţ�");
	int order_id = get_num();
	if (order_id >= order_list.size()) {
		printf("û�д���ţ�\n");
		return;
	}

	auto del = order_list.begin();
	order_list.erase(del + order_id);

	printf("ɾ�����\n");
}

void SaleGoodsModule::modify_goods_in_list()
{
	printf("������Ĺ����嵥��\n");
	print_order_list(order_list);

	printf("��ѡ����Ҫ�޸ĵ���ţ�");
	int order_id = get_num();
	if (order_id >= order_list.size()) {
		printf("û�д���ţ�\n");
		return;
	}

	Order& order = order_list[order_id];
	Goods& goods = (*goods_map)[order.goods_id];


	printf("����Ʒ����%d�ݣ���������Ҫ�����������", goods.quantity);
	int new_quantity = get_num();
	if (new_quantity > goods.quantity) {
		printf("��Ǹ��û����ô��������Ʒ��\n");
		return;
	}
}

void SaleGoodsModule::give_up_list()
{
	bool clear = ask_yes_or_no("���Ҫ������ﳵ��", false);
	if (clear) {
		order_list.clear();
		printf("�����\n");
	}
	else {
		printf("�����\n");
	}
}

void SaleGoodsModule::charge_by_list()
{
	printf("������Ĺ����嵥��\n");
	print_order_list(order_list);

	bool charge = ask_yes_or_no("�Ƿ���ˣ�");
	if (!charge) {
		printf("�˳�\n");
		return;
	}

	printf("�����������Ǯ��");
	float money = get_float();

	float should_pay = 0;
	for (auto& order : order_list) {
		should_pay += order.price * order.num;
	}

	if (money < should_pay) {
		printf("�����Ǯ̫���ˣ�\n");
		return;
	}

	for (Order& order : order_list) {
		record_order(order);
	}

	printf("���㣺%lf\n", double(money) - should_pay);
	printf("��ӭ�´ι��٣�\n");

	order_list.clear();

}

void SaleGoodsModule::print_order_list(vector<Order>& order_list)
{
	printf("���\t��Ʒ��\t����\t����\t�۸�\n");
	int i = 0;
	float sum_money = 0;
	for (auto& order : order_list) {
		Goods& goods = goods_map->find(order.goods_id)->second;
		sum_money += order.num * order.price;
		printf("%d\t%s\t%f\t%d\t%f\n", i++, goods.goods_name.c_str(), order.price, order.num, double(order.num) * order.price);
	}
	printf("�ܼƣ�%f\n", sum_money);
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

// ��ӡ����
void SaleGoodsModule::print_account()
{
	if (!check_admin(login_info))
		return;

	// �������棺���۶���������Ա����Ʒ

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

	// ����
	sort(salesperson_rank.begin(), salesperson_rank.end(), cmp);
	sort(goods_rank.begin(), goods_rank.end(), cmp);

	// ��ӡǰ5
	printf("����Ա������\n");
	printf("����\t����\t�ܽ��\n");
	for (int i = 0; i < 4 && i < salesperson_rank.size(); i++) {
		User& user = user_map->find(salesperson_rank[i].first)->second;
		printf("%d. %s\t%f\n", i, user.username.c_str(), salesperson_rank[i].second);
	}
	printf("----------------------\n");
	printf("��Ʒ����������\n");
	printf("����\t��Ʒ��\t�ܽ��\n");
	for (int i = 0; i < 4 && i < goods_rank.size(); i++) {
		Goods& goods = goods_map->find(goods_rank[i].first)->second;
		printf("%d. %s\t%f\n", i, goods.goods_name.c_str(), goods_rank[i].second);
	}
}

void SaleGoodsModule::record_order(Order& order)
{
	time_t time_now = time(0);
	// ���¹��̲���ʧ�ܣ�����ϵͳ����һ��
	// todo: 1.����quantity������⣬��û���㹻������Ʒ���򷵻�ʧ�ܣ�Ҫ�����¹����̰߳�ȫ��
	bool a = db_record_order(time_now, order.goods_id, order.salesperson_id, order.price, order.num);
	bool b = db_decrease_goods_quantity(order.goods_id, order.num);
	goods_map->find(order.goods_id)->second.quantity -= order.num;

	if (!a && b) {
		printf("ϵͳ�ڲ�����ʧ�ܣ�db_record_order: %d, db_decrease_goods_quantity: %d\n", a, b);
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
	printf("�����߼�������Ӧִ�е�����\n");
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
		printf("��ѡ����\n");
		printf("%d. ������Ʒ�������嵥\n", ADD_GOODS_TO_LIST);
		printf("%d. ɾ�������嵥��һ����Ʒ\n", REMOVE_GOODS_IN_LIST);
		printf("%d. �޸���Ʒ����\n", MODIFY_GOODS_IN_LIST);
		printf("%d. ��չ����嵥\n", GIVE_UP_LIST);
		printf("%d. ����\n", CHARGE_BY_LIST);
		printf("%d. ��ӡ���۱�����Ȩ��\n", PRINT_ACCOUNT);
		printf("%d. �˳���ģ��\n", SGM_EXIT);

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
			printf("����������������ԣ�\n");
			continue;
		}
	}
	return 0;
}


