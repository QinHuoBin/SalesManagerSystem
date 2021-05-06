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

    

    // ��Ʒ�嵥�����ڲ�����
    vector<Order> order_list;

    void print_order_list(vector<Order>& order_list);
    // ���б�������Ʒ���Ҳ�������-1�����򷵻�����
    int find_order_in_list(vector<Order>& order_list, int goods_id);
    void record_order(Order& order);

    // ������map����ģ����и���
    map<int, User>* user_map;
    map<int,Goods>* goods_map;



//private:
    int db_record_order(time_t time_now, int goods_id, int salesperson_id, float price, int quantity);
    int db_enquire_all_orders(vector<Order> &all_orders);
    int db_decrease_goods_quantity(int goods_id, int num);
    
    
    // ͨ�� CommandModule �̳�
    virtual int enter_module() override;

};

