#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include "ext.hpp"

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

struct Element
{
    int awake = 2;
    glm::vec4 color;
    int id;
    int weight;
    int hasPhysics;
    float fom;
};