#pragma once
#include"CommandModule.h"
class GoodsManageModule :
    public CommandModule
{   
public:
    struct Goods {
        int id;
        string goods_name;
        float price;
        int quantity;
    };
    // ͨ�� CommandModule �̳�
    virtual int enter_command() override;
};

