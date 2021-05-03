#pragma once
#include"CommandModule.h"
class GoodsManageModule :
    public CommandModule
{   
public:
    
    GoodsManageModule(LoginInfo* info, sqlite3* db);

    void add_goods();
    void remove_goods();
    void modify_goods();
    void list_goods();

//private:
    int db_add_goods(string goods_name,float price,int quantity);
    int db_remove_goods(string goods_name);
    int db_modify_goods(string old_goods_name,string new_goods_name,float new_price,int new_quantity);
    int db_list_goods(vector<Goods> &goods_list);
    int db_find_goods(string goods_name,Goods *goods=NULL);
    
    // Í¨¹ý CommandModule ¼Ì³Ð
    virtual int enter_module() override;
};

