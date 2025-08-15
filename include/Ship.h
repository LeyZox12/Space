#pragma once
#define PI 3.141592653
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <format>
#include <vector>

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class Ship
{
    public:
        Ship();
        void accelerate(float dt);
        void deccelerate(float dt);
        void update(float dt);
        void leftRotate();
        void rightRotate();
        void debugOnScreen(RenderWindow& window, float dt);
        void setPos(vec2 pos);
        void draw(RenderWindow& window);
        void emergencyStop();
        void toggleAdvancedDebug();
        void setThrottle(float val);
        vec2 getPos();
    private:
        Texture spriteTexture;
        RectangleShape sprite;
        vec2 oldPos;
        float throttle = 0.f;
        float accSpeed = 100.f;
        const float maxThrottle = 1000.f;
        float steer = 0.f;
        float oldRot = 0.f;
        bool advancedDebug = false;
        bool brake = false;
        vec2 vel = vec2(0, 0);
        float debugLineMultiplier = 100.f;

};
