#pragma once
#include "CommandModule.h"
class SaleGoodsModule :
	public CommandModule
{
public:

	SaleGoodsModule(LoginInfo* info,
		sqlite3* db,
		map<int, User>* user_map,
		map<int, Goods>* goods_map);

	void add_goods_to_cart();
	void remove_goods_in_cart();
	void modify_goods_in_cart();
	void give_up_cart();
	void charge_by_cart();
	void print_account();



	// ���ﳵ�����ڲ�����
	vector<Order> shopping_cart;

	void print_order_list(vector<Order>& shopping_cart);
	// ���б�������Ʒ���Ҳ�������-1�����򷵻�����
	int find_order_in_list(vector<Order>& shopping_cart, int goods_id);
	void record_order(Order& order);

	// ������map����ģ����и���
	map<int, User>* user_map;
	map<int, Goods>* goods_map;



	//private:
	int db_record_order(time_t time_now, int goods_id, string goods_name, int salesperson_id, string salesperson_name, float price, int quantity);
	int db_enquire_all_orders(vector<Order>& all_orders);
	int db_decrease_goods_quantity(int goods_id, int num);


	// ͨ�� CommandModule �̳�
	virtual int enter_module() override;

};

