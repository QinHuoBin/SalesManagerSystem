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

void SaleGoodsModule::add_goods_to_cart()
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


	printf("��������Ҫ��ӵ�����:");
	int quantity = get_num();
	if (quantity > goods.quantity) {
		printf("��Ǹ��û����ô��������Ʒ��\n");
		return;
	}

	Order order;
	order.goods_id = goods_id;
	order.goods_name = goods.goods_name;
	order.price = goods.price;
	order.num = quantity;
	order.salesperson_id = login_info->user.id;
	order.salesperson_name = login_info->user.username;

	int loc = find_order_in_list(shopping_cart, goods_id);
	if (loc == -1) {
		shopping_cart.push_back(order);
	}
	else {
		shopping_cart[loc].num += quantity;
	}

	printf("������\n");
}

void SaleGoodsModule::remove_goods_in_cart()
{
	printf("���ǹ��ﳵ�е���Ʒ��\n");
	print_order_list(shopping_cart);
	printf("��ѡ����Ҫɾ������ţ�");
	int order_id = get_num();
	if (order_id >= shopping_cart.size()) {
		printf("û�д���ţ�\n");
		return;
	}

	auto del = shopping_cart.begin();
	shopping_cart.erase(del + order_id);

	printf("ɾ�����\n");
}

void SaleGoodsModule::modify_goods_in_cart()
{
	printf("���ǹ��ﳵ�е���Ʒ��\n");
	print_order_list(shopping_cart);

	printf("��ѡ����Ҫ�޸ĵ���ţ�");
	int order_id = get_num();
	if (order_id >= shopping_cart.size()) {
		printf("û�д���ţ�\n");
		return;
	}

	Order& order = shopping_cart[order_id];
	Goods& goods = (*goods_map)[order.goods_id];


	printf("����Ʒ����%d�ݣ���������Ҫ�����������", goods.quantity);
	int new_quantity = get_num();
	if (new_quantity > goods.quantity) {
		printf("��Ǹ��û����ô��������Ʒ��\n");
		return;
	}
	order.num = new_quantity;
}

void SaleGoodsModule::give_up_cart()
{
	bool clear = ask_yes_or_no("���Ҫ������ﳵ��", false);
	if (clear) {
		shopping_cart.clear();
		printf("�����\n");
	}
	else {
		printf("�����\n");
	}
}

void SaleGoodsModule::charge_by_cart()
{
	printf("���ǹ��ﳵ�е���Ʒ��\n");
	print_order_list(shopping_cart);



	bool charge = ask_yes_or_no("�Ƿ���ˣ�");
	if (!charge) {
		printf("������ֹ\n");
		return;
	}

	printf("�����������Ǯ��");
	float money = get_float();

	float should_pay = 0;
	for (auto& order : shopping_cart) {
		should_pay += order.price * order.num;
	}

	if (money < should_pay) {
		printf("�����Ǯ̫���ˣ�\n");
		return;
	}

	for (Order& order : shopping_cart) {
		record_order(order);
	}

	printf("���㣺%lf\n", double(money) - should_pay);
	printf("��ӭ�´ι��٣�\n");

	shopping_cart.clear();

}

void SaleGoodsModule::print_order_list(vector<Order>& shopping_cart)
{
	printf("���\t��Ʒ��\t����\t����\t�۸�\n");
	int i = 0;
	int sum = 0;
	float sum_money = 0;
	for (auto& order : shopping_cart) {
		Goods& goods = goods_map->find(order.goods_id)->second;
		sum += order.num;
		sum_money += order.num * order.price;
		printf("%d\t%s\t%f\t%d\t%f\n", i++, goods.goods_name.c_str(), order.price, order.num, double(order.num) * order.price);
	}
	printf("�ܼƣ�%d����Ʒ��%fԪ\n", sum, sum_money);
}

int SaleGoodsModule::find_order_in_list(vector<Order>& shopping_cart, int goods_id) {
	for (int i = 0; i < shopping_cart.size(); i++) {
		Order& order = shopping_cart[i];
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
		// �ҵ�������Ա�ڶ����ж�Ӧ��id
		// ���ڸ�λ������Ա��id
		int the_id = salesperson_rank[i].first;
		// �������ж������ж϶���������Աid���Ӧid�Ƿ���ȣ�����ȣ����ҵ�������Ա
		for (int x = all_orders.size() - 1; x >= 0; x--) {
			if (all_orders[x].salesperson_id == the_id) {
				printf("%d. %s\t%f\n", i + 1, all_orders[x].salesperson_name.c_str(), salesperson_rank[i].second);
				break;
			}
		}
	}
	printf("----------------------\n");
	printf("��Ʒ����������\n");
	printf("����\t��Ʒ��\t�ܽ��\n");
	for (int i = 0; i < 4 && i < goods_rank.size(); i++) {
		int the_id = goods_rank[i].first;
		for (int x = all_orders.size() - 1; x >= 0; x--) {
			if (all_orders[x].goods_id == the_id) {
				printf("%d. %s\t%f\n", i + 1, all_orders[x].goods_name.c_str(), goods_rank[i].second);
				break;
			}
		}
	}
}

void SaleGoodsModule::record_order(Order& order)
{
	time_t time_now = time(0);
	// ���¹��̲���ʧ�ܣ�����ϵͳ����һ��
	// todo: 1.����quantity������⣬��û���㹻������Ʒ���򷵻�ʧ�ܣ�Ҫ�����¹����̰߳�ȫ��
	bool a = db_record_order(time_now, order.goods_id, order.goods_name, order.salesperson_id, order.salesperson_name, order.price, order.num);
	bool b = db_decrease_goods_quantity(order.goods_id, order.num);
	goods_map->find(order.goods_id)->second.quantity -= order.num;

	if (!a && b) {
		printf("ϵͳ�ڲ�����ʧ�ܣ�db_record_order: %d, db_decrease_goods_quantity: %d\n", a, b);
	}
}


int SaleGoodsModule::db_record_order(time_t time_now, int goods_id, string goods_name, int salesperson_id, string salesperson_name, float price, int quantity)
{
	string sql = "INSERT INTO Goods_order (id,time,goods_id,goods_name,salesperson_id,salesperson_name,price,quantity) "\
		"VALUES (NULL,?,?,?,?,?,?,?);";

	sqlite3_stmt* stmt;
	int a = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

	a = sqlite3_bind_int(stmt, 1, time_now);
	a = sqlite3_bind_int(stmt, 2, goods_id);
	a = sqlite3_bind_text(stmt, 3, goods_name.c_str(), goods_name.length(), SQLITE_STATIC);
	a = sqlite3_bind_int(stmt, 4, salesperson_id);
	a = sqlite3_bind_text(stmt, 5, salesperson_name.c_str(), salesperson_name.length(), SQLITE_STATIC);
	a = sqlite3_bind_double(stmt, 6, price);
	a = sqlite3_bind_int(stmt, 7, quantity);

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

int SaleGoodsModule::db_enquire_all_orders(vector<Order>& shopping_cart)
{
	string sql = "SELECT id,time,goods_id,goods_name,salesperson_id,salesperson_name,price,quantity FROM Goods_order;";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
	while (1) {
		int rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			Order order;
			order.id = sqlite3_column_int(stmt, 0);
			order.time = sqlite3_column_int(stmt, 1);
			order.goods_id = sqlite3_column_int(stmt, 2);
			order.goods_name = (char*)sqlite3_column_text(stmt, 3);
			order.salesperson_id = sqlite3_column_int(stmt, 4);
			order.salesperson_name = (char*)sqlite3_column_text(stmt, 5);
			order.price = sqlite3_column_double(stmt, 6);
			order.num = sqlite3_column_int(stmt, 7);
			shopping_cart.push_back(order);
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
		printf("������Ʒ����ģ�飬��ѡ����\n");
		printf("%d. ������Ʒ�����ﳵ\n", ADD_GOODS_TO_LIST);
		printf("%d. ɾ�����ﳵ��һ����Ʒ\n", REMOVE_GOODS_IN_LIST);
		printf("%d. �޸���Ʒ����\n", MODIFY_GOODS_IN_LIST);
		printf("%d. ��չ��ﳵ\n", GIVE_UP_LIST);
		printf("%d. ����\n", CHARGE_BY_LIST);
		printf("%d. ��ӡ���۱�����Ȩ��\n", PRINT_ACCOUNT);
		printf("%d. �˳���ģ��\n", SGM_EXIT);

		int cmd = get_num();
		printf("\n");
		switch (cmd) {
		case ADD_GOODS_TO_LIST:add_goods_to_cart(); break;
		case REMOVE_GOODS_IN_LIST:remove_goods_in_cart(); break;
		case MODIFY_GOODS_IN_LIST:modify_goods_in_cart(); break;
		case GIVE_UP_LIST:give_up_cart(); break;
		case CHARGE_BY_LIST:charge_by_cart(); break;
		case PRINT_ACCOUNT:print_account(); break;
		case SGM_EXIT:return -1; break;
		default:
			printf("����������������ԣ�\n");
		}
		printf("\n");
	}
	return 0;
}