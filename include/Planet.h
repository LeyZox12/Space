#pragma once

#define PI 3.141592653

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace std;
using namespace sf;

typedef Vector2f vec2;

class Planet
{
    public:
        Planet(vec2 pos, float radius);
        void updatePoints();
        void setPos(vec2 pos);
        vec2 getPos();
        float getRadius();
        void setPoint(int index, float newDist);
        int getClosestPoint(vec2 pos);
        void draw(RenderWindow& window);
    private:
        int pointCount = 20;
        ConvexShape sprite;
        vec2 pos;
        float radius;
};
