#include <iostream>
#include "api/api.h"

int main() {

    Recivedata api;

    Solar s = api.getSolar();

    std::cout << "Ptotal = " << s.Ptotal << std::endl;

    return 0;
}

//g++ main.cpp api/*.cpp -I. -o app.exe -lcurl