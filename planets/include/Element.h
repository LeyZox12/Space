#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>

using namespace sf;
using namespace std;

struct ContactContext;

class Element
{
    public:
        Element(string displayName, int weight, bool hasPhysics, float fom, Color color);
        Element(string displayName, int weight, bool hasPhysics, float fom, Color color, vector<function<void(ContactContext)>> behaviourOnContact);
        string getName();
        Color getColor();
        int getId();
        int getContactFunctionCount();
        bool getHasPhysics();
        float getFom();
        function<void(ContactContext)> getContactFunction(int index);
        int id;
    private:
        string displayName;
        Color color;
        int weight;
        bool hasPhysics;
        float fom;
        vector<function<void(ContactContext)>> behaviourOnContact;
};