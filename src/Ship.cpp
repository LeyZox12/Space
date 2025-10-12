#include "Ship.h"

Ship::Ship()
{
    if(!spriteTexture.loadFromFile("res/ship/base.png"))
        cout << "error when loading texture\n";
    sprite.setTexture(&spriteTexture);
    sprite.setSize({150, 300});
    sprite.setOrigin({sprite.getSize().x / 2.f, sprite.getSize().y / 2.f});
    float landRad = 10.f;
    leftLandingGear.setRadius(landRad);
    rightLandingGear.setRadius(landRad);
    leftLandingGear.setOrigin({landRad, landRad});
    rightLandingGear.setOrigin({landRad, landRad});
}

void Ship::accelerate(float dt)
{
    throttle += dt * accSpeed;
    if(throttle > maxThrottle) throttle = maxThrottle;
    if(landed)
    {
        landing = false;
        landed = false;
        leftLandingDist-=2.f;
        rightLandingDist-=2.f;
    }
}

void Ship::deccelerate(float dt)
{
    throttle -= dt * accSpeed;
    if(throttle < -maxThrottle) throttle = -maxThrottle;
}


vec2 rotate(vec2 vec, vec2 center, float angle)
{

    vec -= center;
    vec = vec2(vec.x * cos(angle) - vec.y * sin(angle),
               vec.x * sin(angle) + vec.y * cos(angle));
    return vec + center;
}

bool rectCircleColl(RectangleShape rect, CircleShape cir)
{
    vec2 p = rect.getPosition();
    vec2 point = cir.getPosition();
    point = rotate(point, rect.getPosition(), -rect.getRotation().asRadians());
    float rad = cir.getRadius();
    vec2 s = rect.getSize();
    return (abs(p.x - point.x) < rad + s.x * 0.5f && abs(p.y - point.y) < rad + s.y * 0.5f);
}

void Ship::update(vector<Planet> planets, float dt)
{
    float rot = sprite.getRotation().asDegrees();
    float newRot = rot * 2.f - oldRot + steer * dt * dt;
    sprite.setRotation(degrees(newRot));    
    steer *= 0.9f;

    vec2 pos = sprite.getPosition();
    vec2 accDir = vec2(cos(sprite.getRotation().asRadians() - PI/2.f), sin(sprite.getRotation().asRadians() - PI/2.f));
    vel += accDir * throttle;
    vec2 newPos = pos * 2.f - oldPos + vel * dt * dt;
    vel -= accDir * throttle;
    sprite.setPosition(newPos);
    oldPos = pos;
    oldRot = rot;
    auto it = min_element(planets.begin(), planets.end(), [pos](auto& p1, auto& p2)
            {
                return hypot(p1.getPos().x - pos.x, p1.getPos().y - pos.y) < 
                       hypot(p2.getPos().x - pos.x, p2.getPos().y - pos.y); 
            });
    int index = distance(planets.begin(), it);
    currentPlanetIndex = index;
    return;
    /*
    vec2 pos = sprite.getPosition();
    vec2 diff = planets[index].getPos() - sprite.getPosition();
    //float planetDist = hypot(diff.x, diff.y) - planets[index].getRad();
    vec2 leftDiff = leftLandingGear.getPosition() - planets[index].getPos();
    vec2 rightDiff = rightLandingGear.getPosition() - planets[index].getPos();
    float leftDistFromPlanet = hypot(leftDiff.x, leftDiff.y);
    float rightDistFromPlanet = hypot(rightDiff.x, rightDiff.y);
    //float rad = planets[index].getRad();
    float angle = sprite.getRotation().asRadians();
    vec2 straight = vec2(cos(angle), sin(angle));
    
    float trueDist = hypot(diff.x, diff.y);
    if(trueDist > 0.f)
        diff /= trueDist;
    float dot = straight.x * diff.x + straight.y * diff.y;
    diff *= trueDist;
    if(planetDist < 200.f && dot < 0.1 && dot > -0.1)landing = true;
    else landing = false;
    if(landing && landed == false)
    {
        leftLandingDist++;
        rightLandingDist++;
        if(leftDistFromPlanet < rad || rightDistFromPlanet < rad)
        {
            landed = true;
            landing = false;
        }
    }
    else if(!landing)
    {
        leftLandingDist--;
        rightLandingDist--;
        if(leftLandingDist < 0.f)leftLandingDist = 0.f;
        if(rightLandingDist < 0.f)rightLandingDist = 0.f;
    }
    if(landed && leftDistFromPlanet > rad) leftLandingDist++;
    else if(landed && rightDistFromPlanet > rad) rightLandingDist++;
    for(auto& planet : planets)
    {
        vec2 diff = planet.getPos() - pos;
        float dist = hypot(diff.x, diff.y);
        if(dist > 0.f)
        {
            diff /= dist;
            //vel += diff * (float)GRAVITY * (planet.getRad()/(dist * dist));
        }
    }
    
    if(landed)
    {
        return;
    }

    //rotation update
    float rot = sprite.getRotation().asDegrees();
    float newRot = rot * 2.f - oldRot + steer * dt * dt;
    sprite.setRotation(degrees(newRot));    
    steer *= 0.9f;
    //position update
    vec2 accDir = vec2(cos(sprite.getRotation().asRadians() - PI/2.f), sin(sprite.getRotation().asRadians() - PI/2.f));
    vel += accDir * throttle;
    vec2 newPos = pos * 2.f - oldPos + vel * dt * dt;
    vel -= accDir * throttle;
    sprite.setPosition(newPos);
    oldPos = pos;
    oldRot = rot;

    //planet / ship collision
    /*CircleShape planet(planets[index].getRad());
    planet.setPosition(planets[index].getPos());
    planet.setOrigin({planet.getRadius(), planet.getRadius()});
    if(rectCircleColl(sprite, planet))
    {
        sprite.setPosition(pos);
        vel = vec2(0, 0);
    }*/
    

}

void Ship::leftRotate()
{
    steer = -30.f;
}

void Ship::rightRotate()
{
    steer = 30.f;
}

void Ship::draw(RenderTexture& window)
{
    float width = 7.f;
    RectangleShape line({leftLandingDist, width});
    line.setOrigin({0, width * 0.5f});
    float angle = sprite.getRotation().asRadians();
    vec2 pos = sprite.getPosition();
    line.setPosition(pos);

    leftLandingGear.setPosition(pos);
    leftLandingGear.move({cos(angle + PI/3.f) * leftLandingDist,
                          sin(angle + PI/3.f) * leftLandingDist});
    line.setRotation(radians(angle + PI/3.f));
    window.draw(leftLandingGear);
    window.draw(line);
    rightLandingGear.setPosition(pos);
    rightLandingGear.move({cos(angle + PI*0.66f) * rightLandingDist,
                           sin(angle + PI*0.66f) * rightLandingDist});
    line.setSize({rightLandingDist, width*0.5f});
    line.setRotation(radians(angle + PI*0.66f));
    window.draw(line);
    window.draw(rightLandingGear);
    window.draw(sprite);
}

void Ship::debugOnScreen(RenderWindow& window, float dt)
{
    Font f("res/font.ttf");
    Text t(f, "", 24);
    t.setPosition(window.mapPixelToCoords({0, 0}));

    vec2 diff = sprite.getPosition() - oldPos;
    float rotDiff = -sprite.getRotation().asDegrees() - oldRot;
    string debugStr = "pos: \nx:" + format("{:.1f}",sprite.getPosition().x) + "\ny:" + format("{:.1f}",sprite.getPosition().y) + "\nthrottle:" + format("{:.1f}",throttle) + "\nsteer:" + format("{:.1f}",steer) + "\nmag:" + format("{:.1f}", hypot(diff.x, diff.y)) + "\n";
    if(brake)
    {
        debugStr += "EMERGENCY BRAKE ON\n";
        brake = false;
    }
    if(landed) debugStr += "LANDED\n";
    else if(landing) debugStr += "LANDING\n";
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
    if(mag > 0.f) diff /= mag;
    vel = vec2(0,0);
    oldPos += diff * 0.05f;
    throttle = 0;
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

bool Ship::getLanding()
{
    return landing;
}

bool Ship::getLanded()
{
    return landed;
}

int Ship::getCurrentPlanet()
{
    return currentPlanetIndex;
}

vec2 Ship::getPos()
{
    return sprite.getPosition();
}
