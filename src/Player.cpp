#include "Player.h"

Player::Player(Point* p)
:point(p)
{
    if(!baseTexture.loadFromFile("res/player/base.png"))
        cout << "couldn't load player base sprite\n";
    if(!eyeTexture.loadFromFile("res/player/eye.png"))
        cout << "couldn't load player eye sprite\n";
    float size = 100.f;
    baseSprite.setSize({size, size * 1.3f});
    eyeSprite.setSize({size*0.75f, size*0.75f});
    baseSprite.setTexture(&baseTexture);
    eyeSprite.setTexture(&eyeTexture);
    baseSprite.setOrigin({baseSprite.getSize().x / 2.f, baseSprite.getSize().y / 2.f});
    eyeSprite.setOrigin({eyeSprite.getSize().x / 2.f, eyeSprite.getSize().y / 2.f});
}

void Player::setPos(vec2 pos)
{
    point->setPos(pos, true);
    point->setOldPos(pos);
}

vec2 rotateMouse(vec2 vec, vec2 center, float angle)
{

    vec -= center;
    vec = vec2(vec.x * cos(angle) - vec.y * sin(angle),
               vec.x * sin(angle) + vec.y * cos(angle));
    return vec + center;
}

void Player::setPoint(Point* p)
{
    point = p;
}

void Player::update(Planet currentPlanet, vec2 mousepos, bool landed, float dt)
{//TODO rotate player according to mouse using rotVel if not landed;
    vec2 rotated = rotateMouse(mousepos, baseSprite.getPosition(), -baseSprite.getRotation().asRadians());
    int xScale = rotated.x < baseSprite.getPosition().x ? -1 : 1;
    baseSprite.setScale({xScale, 1});
    eyeSprite.setScale({xScale, 1});
    vec2 diff = baseSprite.getPosition() - currentPlanet.getPos();
    float dist = hypot(diff.x, diff.y);
    if(dist > 0.f) diff /= dist;
    float angle = atan2(diff.y, diff.x) + PI;
    baseSprite.setRotation(radians(angle-PI/2.f));
    eyeSprite.setRotation(radians(angle-PI/2.f));
    float minDist = currentPlanet.getRad() + baseSprite.getSize().y / 2.f;
    point->addAcc(9.8f * vec2(cos(angle), sin(angle)));
    vec2 pos = point->getPos();
    if(dist < minDist)
    {
        vec2 newPos = currentPlanet.getPos() + vec2(cos(angle-PI), sin(angle-PI)) * minDist;
        point->setPos(newPos, true);
        point->setOldPos(newPos);
        point->setAcc(vec2(0, 0));
    }
    baseSprite.setPosition(point->getPos());
    eyeSprite.setPosition(point->getPos() - vec2(cos(angle)*baseSprite.getSize().y * 0.15f, sin(angle)*baseSprite.getSize().y * 0.15f));
}

void Player::addVel(vec2 vel)
{
    this->point->addAcc(vel);
}

void Player::draw(RenderTexture& window)
{
    window.draw(eyeSprite);
    window.draw(baseSprite);
}

void Player::drawUI(RenderWindow& window)
{

}

float Player::getRot()
{
    return baseSprite.getRotation().asRadians();
}

vec2 Player::getPos()
{
    return baseSprite.getPosition();
}

void Player::move(vec2 delta)
{
    point->move(delta, true);
}

void Player::setVel(vec2 vel)
{
    point->setAcc(vel);
}
