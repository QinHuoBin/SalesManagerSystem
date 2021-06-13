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



	// 购物车，由内部控制
	vector<Order> shopping_cart;

	void print_order_list(vector<Order>& shopping_cart);
	// 在列表中找商品，找不到返回-1，否则返回索引
	int find_order_in_list(vector<Order>& shopping_cart, int goods_id);
	void record_order(Order& order);

	// 以下两map由主模块进行更新
	map<int, User>* user_map;
	map<int, Goods>* goods_map;



	//private:
	int db_record_order(time_t time_now, int goods_id, string goods_name, int salesperson_id, string salesperson_name, float price, int quantity);
	int db_enquire_all_orders(vector<Order>& all_orders);
	int db_decrease_goods_quantity(int goods_id, int num);


	// 通过 CommandModule 继承
	virtual int enter_module() override;

};

