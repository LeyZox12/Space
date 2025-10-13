#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "planet.h"
#define PI 3.141592653

using namespace sf;
using namespace std;

typedef Vector2f vec2 ;

class Planet;

class Map
{
    public:
        Map(vec2 posDisabled, vec2 posEnabled, vec2 spriteSize);
        void setPos(vec2 posDisabled, vec2 posEnabled);
        void draw(RenderWindow& window, vector<Planet> planets, vec2 mousepos, vec2 shipPos);
        bool onClick(vec2 mousepos);
        void toggle();
        void update(RenderWindow& window, vec2 shipPos, float dt);
        void onScroll(int delta);
    private:
        Texture mapTexture;
        Texture pinTexture;
        RectangleShape mapSprite;
        RectangleShape pinSprite;
        bool active = false;
        bool animating = false;
        bool pin = false;
        vec2 pinPos;
        vec2 pinRatio;
        clock_t animationStart;
        float animationDuration = 1.f;
        float mapDistance = 100000.f;
        vec2 posDisabled;
        vec2 posEnabled;
};
