#include "Planet.h"



Planet::Planet(vec2 pos, float radius)
{
    this->pos = pos;
    this->radius = radius;
    pointCount = (int)round(radius/20.f);
    sprite.setPointCount(pointCount);
    updatePoints();
}

void Planet::updatePoints()
{
    float angleRatio = PI * 2.f / (float)pointCount;
    for(int i = 0; i < pointCount; i++)
    {
        vec2 pointPos =pos + vec2(cos(angleRatio * i) * radius, sin(angleRatio * i) * radius);
        sprite.setPoint(i, pointPos);
    }
    sprite.setFillColor(Color::Green);
}

void Planet::setPos(vec2 pos)
{
    this -> pos = pos;
    updatePoints();
}

vec2 Planet::getPos()
{
    return pos;
}

float Planet::getRad()
{
    return radius;
}

void Planet::setPoint(int index, float newDist)
{
    float angle = PI * 2.f / (float) pointCount * index;
    vec2 newPos = vec2(cos(angle) * newDist, sin(angle) * newDist);
    sprite.setPoint(index, newPos);
}

int Planet::getClosestPoint(vec2 pos)
{
    float closestDist = hypot(pos.x - sprite.getPoint(0).x, pos.y - sprite.getPoint(0).y);
    int closestIndex = 0;
    for(int i = 1; i < pointCount; i++)
    {
        vec2 diff = pos - sprite.getPoint(i); 
        float dist = hypot(diff.x, diff.y);
        if(dist < closestDist)
        {
            closestDist = dist;
            closestIndex = i;
        }
    }
    return closestIndex;
}

void Planet::draw(RenderWindow& window)
{
    window.draw(sprite);
}
