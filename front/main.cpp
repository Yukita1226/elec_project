#include <iostream>
#include "api/api.h"

int main() {

    Recivedata api;

    Car s = api.getCar();

    std::cout << "Ptotal = " << s.Ptotal << std::endl;
    std::cout << "Ptransfer = " << s.Ptransfer << std::endl;

    return 0;
}

//g++ main.cpp api/*.cpp -I. -o app.exe -lcurl