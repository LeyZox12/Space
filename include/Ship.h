#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#define PI 3.141592653
#define GRAVITY 66.7
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <format>
#include <vector>
#include "Planet.h"

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class Planet;

class Ship
{
    public:
        Ship();
        void accelerate(float dt);
        void deccelerate(float dt);
        void update(vector<Planet>& planets, float dt);
        void leftRotate();
        void rightRotate();
        void debugOnScreen(RenderWindow& window, float dt);
        void setPos(vec2 pos);
        void draw(RenderTexture& window);
        void emergencyStop();
        void toggleAdvancedDebug();
        void setThrottle(float val);
        RectangleShape getSprite();
        bool getLanding();
        Texture& getTexture();
        bool getLanded();
        vec2 getPos();
        int getCurrentPlanet();
    private:
        Texture spriteTexture;
        RectangleShape sprite;
        CircleShape leftLandingGear;
        CircleShape rightLandingGear;
        vec2 oldPos;
        float throttle = 0.f;
        float accSpeed = 100.f;
        const float maxThrottle = 200.f;
        float steer = 0.f;
        float oldRot = 0.f;
        bool advancedDebug = false;
        bool brake = false;
        bool landing = false;
        bool landed = false;
        int currentPlanetIndex = -1;
        vec2 vel = vec2(0, 0);
        float debugLineMultiplier = 100.f;
        float leftLandingDist = 0.f;
        float rightLandingDist = 0.f;
};
