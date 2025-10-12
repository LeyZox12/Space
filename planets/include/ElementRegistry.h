#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Element.h"
#include <functional>

using namespace std;
using namespace sf;

typedef Vector2f vec2;


struct ElementRegistry
{
    vector<Element> elements;
    
    ElementRegistry()
    {
        registerElement(Element("NONE_ERROR", 0, false, 0.f, Color(255, 0, 218)));
    }

    Element getElementByName(string name)
    {
        for(auto& e : elements)
        {
            if(e.getName() == name) return e;
        }
        return elements[0];
    }

    Element getElementByColor(Color color)
    {
        for(auto& e : elements)
            if(e.getColor().r == color.r && 
               e.getColor().g == color.g &&
               e.getColor().b == color.b) return e;
        return elements[0];
    }

    void registerElement(Element e)
    {
        e.id = elements.size();
        elements.emplace_back(e);
    }
};