#include "../struct/car.h"
#include "../struct/solar.h"
#include "../struct/evstation.h"
#pragma once

class Recivedata {
public:
    Solar       getSolar();
    Car         getCar();
    Evstation   getEv();
};
