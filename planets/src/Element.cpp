#include "Element.h"

Element::Element(string displayName, int weight, bool hasPhysics, float fom, Color color)
:displayName(displayName), weight(weight), hasPhysics(hasPhysics), fom(fom), color(color)
{

}

Element::Element(string displayName, int weight, bool hasPhysics, float fom, Color color, vector<function<void(ContactContext)>> behaviourOnContact)
:displayName(displayName), weight(weight), hasPhysics(hasPhysics), fom(fom), color(color), behaviourOnContact(behaviourOnContact)
{

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