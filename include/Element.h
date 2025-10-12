#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>

using namespace sf;
using namespace std;

struct ContactContext;

struct TemperatureProfile
{
    TemperatureProfile()
    {
        canChangeState = false;
    }
    TemperatureProfile(float minT, int stateChangeOnMin, float maxT, int stateChangeOnMax)
    :minT(minT), maxT(maxT), stateChangeOnMax(stateChangeOnMax), stateChangeOnMin(stateChangeOnMin)
    {
        canChangeState = true;
    }
    bool canChangeState;
    int stateChangeOnMin;
    int stateChangeOnMax;
    float minT = -273.15;
    float maxT = 5000.f;
};

class Element
{
    public:
        int awake = 2;
        Element(string displayName, int weight, bool hasPhysics, float fom, Color color);
        Element(string displayName, int weight, bool hasPhysics, float fom, Color color, vector<function<void(ContactContext)>> behaviourOnContact);
        string getName();
        Color getColor();
        int getId();
        int getContactFunctionCount();
        bool getHasPhysics();
        int getWeight();
        float getFom();
        void shiftColor();
        function<void(ContactContext)> getContactFunction(int index);
        int id;
    private:
        string displayName;
        Color color;
        int weight;
        bool hasPhysics;
        float fom;
        TemperatureProfile profile;
        vector<function<void(ContactContext)>> behaviourOnContact;
};