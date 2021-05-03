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

    void add_goods_to_list();
    void remove_goods_in_list();
    void modify_goods_in_list();
    void give_up_list();
    void charge_by_list();
    void print_account();

    

    // 商品清单，由内部控制
    vector<Order> order_list;

    void print_order_list(vector<Order>& order_list);
    // 在列表中找商品，找不到返回-1，否则返回索引
    int find_order_in_list(vector<Order>& order_list, int goods_id);
    void record_order(Order& order);

    // 以下两map由主模块进行更新
    map<int, User>* user_map;
    map<int,Goods>* goods_map;



//private:
    int db_record_order(time_t time_now, int goods_id, int salesperson_id, float price, int quantity);
    int db_enquire_all_orders(vector<Order> &all_orders);
    int db_decrease_goods_quantity(int goods_id, int num);
    
    
    // 通过 CommandModule 继承
    virtual int enter_module() override;

};

