#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;
using namespace std;

typedef Vector2f vec2;


struct Camera
{
    Camera(vec2 size, float speed)
    :speed(speed), view({0, 0}, size) {}

    void update(vec2 target)
    {
        vec2 diff = target - view.getCenter();
        float mag = hypot(diff.x, diff.y);
        if(mag > 0.f) diff /= 100.f / speed;
        view.move(diff * speed);
    }
    
    View& getView()
    {
        return view;
    }

    void setSize(vec2 size)
    {
        view.setSize(size);
    }
    
    vec2 getPos()
    {
        return view.getCenter();
    }

    vec2 getSize()
    {
        return view.getSize();
    }

    float speed = 1.f;
    View view;
};
