#pragma once
#include"SalesManagerSystem.h"
int main()
{
    SalesManagerSystem sys = SalesManagerSystem();
    

    User user{ -1, "Jonh", "Doe", 664416000, std::make_unique<std::string>("url_to_heaven"), 3 };

    auto insertedId = storage.insert(user);
    cout << "insertedId = " << insertedId << endl;      //  insertedId = 8
    user.id = insertedId;

    User secondUser{ -1, "Alice", "Inwonder", 831168000, {} , 2 };
    insertedId = storage.insert(secondUser);
    secondUser.id = insertedId;
    std::cout << "Hello World!\n";
}