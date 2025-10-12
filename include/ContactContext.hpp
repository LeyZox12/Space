#pragma once
#include <iostream>
#include <vector>
#include "Element.h"

using namespace std;
class Planet;

struct ElementRegistry;

struct ContactContext
{
    ContactContext(Planet& planet, Element contact, vector<uint8_t>& buffer, vector<Element>& idBuffer, ElementRegistry& er, int selfX, int selfY, int contactX, int contactY)
    :planet(planet), contact(contact), buffer(buffer), idBuffer(idBuffer), selfX(selfX), selfY(selfY), contactX(contactX), contactY(contactY), er(er){}
    Element contact;
    ElementRegistry& er;
    vector<uint8_t>& buffer;
    vector<Element>& idBuffer;
    Planet& planet;
    int selfX, selfY, contactX, contactY;
};