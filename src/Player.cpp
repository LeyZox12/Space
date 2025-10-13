#include "Player.h"

Player::Player(Point *p) : point(p) {
  if (!baseTexture.loadFromFile("res/player/base.png"))
    cout << "couldn't load player base sprite\n";
  if (!eyeTexture.loadFromFile("res/player/eye.png"))
    cout << "couldn't load player eye sprite\n";
  float size = 100.f;
  baseSprite.setSize({size, size * 1.3f});
  eyeSprite.setSize({size * 0.75f, size * 0.75f});
  baseSprite.setTexture(&baseTexture);
  eyeSprite.setTexture(&eyeTexture);
  baseSprite.setOrigin(
      {baseSprite.getSize().x / 2.f, baseSprite.getSize().y / 2.f});
  eyeSprite.setOrigin(
      {eyeSprite.getSize().x / 2.f, eyeSprite.getSize().y / 2.f});
}

void Player::setPos(vec2 pos) {
  point->setPos(pos, true);
  point->setOldPos(pos);
}

vec2 rotateMouse(vec2 vec, vec2 center, float angle) {

  vec -= center;
  vec = vec2(vec.x * cos(angle) - vec.y * sin(angle),
             vec.x * sin(angle) + vec.y * cos(angle));
  return vec + center;
}

void Player::setPoint(Point *p) { point = p; }

void Player::update(Planet& currentPlanet, vec2 mousepos, bool landed,
                    float dt) { // TODO rotate player according to mouse using
                                // rotVel if not landed;
  vec2 rotated = rotateMouse(mousepos, baseSprite.getPosition(),
                             -baseSprite.getRotation().asRadians());
  int xScale = rotated.x < baseSprite.getPosition().x ? -1 : 1;
  baseSprite.setScale(vec2(xScale, 1));
  eyeSprite.setScale(vec2(xScale, 1));
  vec2 diff = point->getPos() - currentPlanet.getPos();
  float dist = hypot(diff.x, diff.y);
  if (dist > 0.f)
    diff /= dist;
  if(landed)
  {
    /*point->setAcc(999.8f * -diff);
    float angle = atan2(diff.y, diff.x) + PI;
    baseSprite.setRotation(sf::radians(angle - PI / 2.f));
    eyeSprite.setRotation(sf::radians(angle - PI / 2.f));
    float minDist = currentPlanet.getRad() + baseSprite.getSize().y / 2.f;
    point->addAcc(9.8f * vec2(cos(angle), sin(angle)));
    if (dist < minDist) {
      vec2 newPos = currentPlanet.getPos() +
                    vec2(cos(angle - PI), sin(angle - PI)) * minDist;
      point->setPos(newPos, true);
      point->setOldPos(newPos);
      point->setAcc(vec2(0, 0));
    }
    baseSprite.setPosition(point->getPos());
    eyeSprite.setPosition(point->getPos() -
                          vec2(cos(angle) * baseSprite.getSize().y * 0.15f,
                              sin(angle) * baseSprite.getSize().y * 0.15f));*/
  }
  else 
  {
    vec2 mousediff = baseSprite.getPosition() - mousepos;
    float angle = atan2(mousediff.y, mousediff.x) + PI;
    baseSprite.setRotation(sf::radians(angle));
    eyeSprite.setRotation(sf::radians(angle));
    baseSprite.setPosition(point->getPos());
    eyeSprite.setPosition(point->getPos() -
                          vec2(cos(angle) * baseSprite.getSize().y * 0.15f,
                              sin(angle) * baseSprite.getSize().y * 0.15f));

  }
  vec2 pos = point->getPos();
}

void Player::addVel(vec2 vel) { this->point->addAcc(vel); }

void Player::draw(sf::RenderTexture &window) {
  window.draw(eyeSprite);
  window.draw(baseSprite);
}

void Player::drawUI(sf::RenderWindow &window) {}

float Player::getRot() { return baseSprite.getRotation().asRadians(); }

vec2 Player::getPos() { return baseSprite.getPosition(); }

Point *Player::getPoint() { return point; }

void Player::move(vec2 delta) { point->move(delta, true); }

void Player::setVel(vec2 vel) { point->setAcc(vel); }
