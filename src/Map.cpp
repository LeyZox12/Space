#include "Map.h"

Map::Map(vec2 posDisabled, vec2 posEnabled, vec2 spriteSize)
{
    if(!mapTexture.loadFromFile("res/UI/map.png")) cout << "error when loading map";
    if(!pinTexture.loadFromFile("res/UI/pin.png")) cout << "error when loading pin";
    mapSprite.setTexture(&mapTexture);
    pinSprite.setTexture(&pinTexture);
    this->posDisabled = posDisabled;
    this->posEnabled = posEnabled;
    mapSprite.setSize(spriteSize);
    pinSprite.setSize(spriteSize * 0.1f);
    pinSprite.setOrigin(pinSprite.getSize() * 0.5f);
}

void Map::setPos(vec2 posDisabled, vec2 posEnabled)
{
    this->posDisabled = posDisabled;
    this->posEnabled = posEnabled;
}

void Map::draw(RenderWindow& window, vec2 mousepos, vec2 shipPos)
{
    vec2 pos = mapSprite.getPosition();
    vec2 size = mapSprite.getSize();
    if(mousepos.x > pos.x && 
       mousepos.y > pos.y && 
       mousepos.x < pos.x + size.x &&
       mousepos.y < pos.y + size.y)
    {
        static auto crosshairCur = Cursor(Cursor::Type::Cross);
        window.setMouseCursor(crosshairCur);
    }
    else
    {
        static auto normalCur = Cursor(Cursor::Type::Arrow);
        window.setMouseCursor(normalCur);
    }
    window.draw(mapSprite);
    if(pin)
    {
        vec2 center = mapSprite.getPosition() + mapSprite.getSize() * 0.5f;
        vec2 posOnMap = center + pinRatio * mapSprite.getSize().x * 0.5f;
        pinSprite.setPosition(posOnMap);
        pinSprite.setRotation(radians(0.f));
        window.draw(pinSprite);
        float angle = atan2(pinRatio.y, pinRatio.x);
        pinSprite.setPosition(shipPos + vec2(cos(angle) * 200.f, sin(angle) * 200.f));
        pinSprite.setRotation(radians(angle - PI/2.f));
        window.draw(pinSprite);
        static Font f("res/font.ttf");
        vec2 diff = pinPos - shipPos;
        float distV = hypot(diff.x, diff.y);
        Text dist(f, format("{:.1f}", distV/1000.f) + "km", 20);
        dist.setPosition(pinSprite.getPosition() - vec2(0, 50));
        window.draw(dist);
    }
}

void Map::onClick(vec2 mousepos)
{
    vec2 ratio = (mousepos - (mapSprite.getPosition()+mapSprite.getSize()/ 2.f))/(mapSprite.getSize().x*0.5f);
    pinRatio = ratio;
    pinPos = pinRatio * mapDistance;
    pin = true;
}

void Map::toggle()
{
    animationStart = clock();
    animating = true;
    active = !active;
}

void Map::update(RenderWindow& window, vec2 shipPos, float dt)
{
    vec2 origin = active ? posDisabled: posEnabled;
    vec2 target = active ? posEnabled : posDisabled;
    origin = window.mapPixelToCoords(Vector2i(origin.x, origin.y));
    target = window.mapPixelToCoords(Vector2i(target.x, target.y));
    if(animating)
    {
        vec2 delta = target - origin;
        float ratio = (clock() - animationStart) / (animationDuration * 1000.f);
        mapSprite.setPosition(origin + (delta * ratio));
        if(ratio >= 1.f)animating = false;
    }
    else
    {
        mapSprite.setPosition(target);
    }
    if(pin)
    {
        pinRatio = (pinPos - shipPos) / mapDistance;
    }
}
