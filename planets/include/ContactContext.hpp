#pragma once
#include <iostream>
#include <vector>
#include "Element.h"

using namespace std;

class Planet;

struct ContactContext
{
    ContactContext(Planet& planet, Element contact, vector<uint8_t>& buffer, int selfX, int selfY, int contactX, int contactY)
    :planet(planet), contact(contact), buffer(buffer), selfX(selfX), selfY(selfY), contactX(contactX), contactY(contactY){}
    Element contact;
    vector<uint8_t>& buffer;
    Planet& planet;
    int selfX, selfY, contactX, contactY;
};