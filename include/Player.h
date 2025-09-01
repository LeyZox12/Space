#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Planet.h"
#include "Point.h"
#include <cmath>
#define PI 3.141592653

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class Point;
class Planet;

class Player
{
    public:
        Player(Point* p);
        void setPos(vec2 pos);
        void update(Planet currentPlanet, vec2 mousepos, bool landed, float dt);
        void setVel(vec2 vel);
        void addVel(vec2 vel);
        void draw(RenderTexture& window);
        void drawUI(RenderWindow& window);
        void setPoint(Point* p);
        float getRot();
        vec2 getPos();
        void move(vec2 delta);
    private:
        Texture baseTexture;
        Texture eyeTexture;
        RectangleShape eyeSprite;
        Point* point;
        RectangleShape baseSprite;
        float rotVel;
        vec2 vel;
        vec2 oldPos;
};
