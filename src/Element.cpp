#include "Element.h"
#include <algorithm>

Element::Element(string displayName, int weight, bool hasPhysics, float fom, Color color)
:displayName(displayName), weight(weight), hasPhysics(hasPhysics), fom(fom), color(color)
{

}

Element::Element(string displayName, int weight, bool hasPhysics, float fom, Color color, vector<function<void(ContactContext)>> behaviourOnContact)
:displayName(displayName), weight(weight), hasPhysics(hasPhysics), fom(fom), color(color), behaviourOnContact(behaviourOnContact)
{

}

int Element::getWeight()
{
    return weight;
}

void Element::shiftColor()
{
    float shiftRatio =1.0 +  (rand()%100 / 400.f);
    float grayScale = (0.2126f * (float)color.r+
                      0.7152 * (float)color.g + 
                      0.0722 * (float)color.b) * shiftRatio;

    int r = color.r * shiftRatio;
    int g = color.g * shiftRatio;
    int b = color.b * shiftRatio;

    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    color = sf::Color(r,g,b);

}

string Element::getName()
{
    return displayName;
}

Color Element::getColor()
{
    return color;
}

int Element::getId()
{
    return id;
}

bool Element::getHasPhysics()
{
    return hasPhysics;
}

float Element::getFom()
{
    return fom;
}

int Element::getContactFunctionCount()
{
    return behaviourOnContact.size();
}

function<void(ContactContext)> Element::getContactFunction(int index)
{
    return behaviourOnContact[index];
}