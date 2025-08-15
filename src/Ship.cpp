#include <Ship.h>

Ship::Ship()
{
    if(!spriteTexture.loadFromFile("res/ship/base.png"))
        cout << "error when loading texture\n";
    sprite.setTexture(&spriteTexture);
    sprite.setSize({150, 300});
    sprite.setOrigin({sprite.getSize().x / 2.f, sprite.getSize().y / 2.f});
}

void Ship::accelerate(float dt)
{
    throttle += dt * accSpeed;
    if(throttle > maxThrottle) throttle = maxThrottle;
}

void Ship::deccelerate(float dt)
{
    throttle -= dt * accSpeed;
    if(throttle < -maxThrottle) throttle = -maxThrottle;
}

void Ship::update(float dt)
{
    //rotation update
    float rot = sprite.getRotation().asDegrees();
    float newRot = rot * 2.f - oldRot + steer * dt * dt;
    sprite.setRotation(degrees(newRot));    
    steer *= 0.9f;
    //position update
    vec2 accDir = vec2(cos(sprite.getRotation().asRadians() - PI/2.f), sin(sprite.getRotation().asRadians() - PI/2.f));
    vel += accDir * throttle;
    vec2 pos = sprite.getPosition();
    vec2 newPos = pos * 2.f - oldPos + vel * dt * dt;
    vel -= accDir * throttle;
    sprite.setPosition(newPos);
    oldPos = pos;
    oldRot = rot;
}

void Ship::leftRotate()
{
    steer = -30.f;
}

void Ship::rightRotate()
{
    steer = 30.f;
}

void Ship::draw(RenderWindow& window)
{
    window.draw(sprite);
}

void Ship::debugOnScreen(RenderWindow& window, float dt)
{
    Font f("res/font.ttf");
    Text t(f, "", 24);
    t.setPosition(window.mapPixelToCoords({0, 0}));

    vec2 diff = sprite.getPosition() - oldPos;
    float rotDiff = -sprite.getRotation().asDegrees() - oldRot;
    string debugStr = "pos: \nx:" + format("{:.1f}",sprite.getPosition().x) + "\ny:" + format("{:.1f}",sprite.getPosition().y) + "\nthrottle:" + format("{:.1f}",throttle) + "\nsteer:" + format("{:.1f}",steer) + "\nmag:" + format("{:.1f}", hypot(diff.x, diff.y));
    
    t.setString(debugStr);
    t.setFillColor(Color::White);
    window.draw(t);    
    if(advancedDebug)
    {
        VertexArray line(PrimitiveType::LineStrip, 2);
        line[0].color = Color::Green;
        line[1].color = Color::Green;
        line[0].position = sprite.getPosition();
        line[1].position = line[0].position + diff * debugLineMultiplier;
        window.draw(line);
        float angle = sprite.getRotation().asRadians() - PI/2.f;
       // line[0].position += vec2(cos(angle) * sprite.getSize().y / 2.f,sin(angle) * sprite.getSize().y / 2.f);
        //line[1].position = line[0].position + vec2(cos(angle - PI/2.f) * rotDiff * debugLineMultiplier, sin(angle - PI/2.f) * rotDiff * debugLineMultiplier);
        //window.draw(line);
        if(brake)
        {
            line[0].color = Color::Red;
            line[1].color = Color::Red;
            line[0].position = sprite.getPosition();
            line[1].position = line[0].position - diff * debugLineMultiplier;
            window.draw(line);
            brake = false;
        }
    }
}

void Ship::setPos(vec2 pos)
{
    sprite.setPosition(pos);
    oldPos = pos;
}

void Ship::emergencyStop()
{
    vec2 diff = sprite.getPosition() - oldPos;
    float mag = hypot(diff.x, diff.y);
    vel = -diff * 100.f;
    brake = true; 
}

void Ship::toggleAdvancedDebug()
{
    advancedDebug = !advancedDebug;
}

void Ship::setThrottle(float val)
{
    throttle = val;
}

vec2 Ship::getPos()
{
    return sprite.getPosition();
}
