#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Element.h"
#include <functional>
#include "Item.hpp"

using namespace std;
using namespace sf;

typedef Vector2f vec2;


struct ElementRegistry
{
    vector<Element> elements;
    
    ElementRegistry()
    {
        registerElement("NONE_ERROR", 0, false, 0.f, Color(255, 0, 218));
    }

    Element getElementByColor(Color color)
    {
        for(auto& e : elements)
            if(e.color.r == color.r && 
               e.color.g == color.g &&
               e.color.b == color.b) return e;
        return elements[0];
    }

    Element getElementById(int id)
    {
        if(id < 0 || id > elements.size()) return elements[0];
        return elements[id];
    }

    void registerElement(string name, int weight, bool hasPhysics, float fom, Color color)
    {
        Element e;
        e.weight = weight;
        e.hasPhysics = hasPhysics;
        e.fom = fom;
        e.id = elements.size();
        e.color = glm::vec4(
            (uint8_t)color.r,
            (uint8_t)color.g,
            (uint8_t)color.b,
            (uint8_t)color.a);
        elements.emplace_back(e);
    }
};