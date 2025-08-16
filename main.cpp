#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#define PI 3.141592653
#include "Camera.hpp"
#include "Ship.h"
#include "Map.h"
#include "Planet.h"

using namespace sf;
using namespace std;

typedef Vector2f vec2;

RenderWindow window(VideoMode({512, 512}), "space is cool");
Ship ship;
vector<Keyboard::Key> pressedKeys;
sf::Clock deltaClock;
Camera cam(vec2(window.getSize().x, window.getSize().y), 4.f);
vector<RectangleShape> stars;
vector<Planet> planets;
Map map(vec2(window.getSize().x - 300, window.getSize().y), vec2(window.getSize().x - 300, window.getSize().y - 300), vec2(300, 300));

const int STARSAMOUNT = 100;

void start();
void inputManager(float dt);
void updateStars();

int main()
{
    start();
    while(window.isOpen())
    {
        vec2 mousepos = vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        vec2 mouseposWorld = window.mapPixelToCoords(Vector2i(mousepos.x, mousepos.y));
        float dt = deltaClock.restart().asSeconds();
        while(optional<Event> e = window.pollEvent())
        {
            if(e->is<Event::Closed>())window.close();
            if(e->is<Event::Resized>())
            {
                Vector2u size = e->getIf<Event::Resized>()->size;
                View v({0, 0}, {size.x, size.y});
                window.setView(v);
                cam.setSize(vec2(size.x, size.y));
                stars.clear();
                map.setPos(vec2(window.getSize().x - 300, window.getSize().y), vec2(window.getSize().x - 300, window.getSize().y - 300));
            }
            if(e->is<Event::KeyPressed>())
            {
                Keyboard::Key key = e->getIf<Event::KeyPressed>()->code;
                if(count(pressedKeys.begin(), pressedKeys.end(), key) == 0)pressedKeys.push_back(key);
            }
            else if(e->is<Event::KeyReleased>())
            {
                Keyboard::Key key = e->getIf<Event::KeyReleased>()->code;
                if(count(pressedKeys.begin(), pressedKeys.end(), key) == 1)pressedKeys.erase(find(pressedKeys.begin(), pressedKeys.end(), key));
                if(key == Keyboard::Key::F3) ship.toggleAdvancedDebug();
                else if(key == Keyboard::Key::LShift) ship.setThrottle(0.f);
                else if(key == Keyboard::Key::Tab) map.toggle();
            }
            else if(e->is<Event::MouseButtonPressed>())
            {
                Mouse::Button button = e->getIf<Event::MouseButtonPressed>()->button;
                switch(button)
                {
                    case(Mouse::Button::Left):
                        map.onClick(mouseposWorld);
                        break;
                }
            }
        }

        inputManager(dt);
        cam.update(ship.getPos(), window);
        map.update(window, ship.getPos(), dt);
        ship.update(planets, dt);
        window.clear(Color::Black);
        updateStars();
        ship.draw(window);
        for(auto& planet : planets)planet.draw(window);
        ship.debugOnScreen(window, dt);
        map.draw(window, mouseposWorld, ship.getPos());
        window.display();
    }
}

void start()
{
    ship.setPos(vec2(256, 256));
    window.setFramerateLimit(60);
    planets.emplace_back(Planet(vec2(-3000, 2000), 600.f));
    planets.emplace_back(Planet(vec2(1300, -2000), 600.f));
    planets.emplace_back(Planet(vec2(1200, 0), 600.f));
}

void inputManager(float dt)
{
    for(auto& key : pressedKeys)
    switch(key)
    {
        case(Keyboard::Key::Z):
            ship.accelerate(dt);
            break;
        case(Keyboard::Key::S):
            ship.deccelerate(dt);
            break;
        case(Keyboard::Key::Q):
            ship.leftRotate();
            break;
        case(Keyboard::Key::D):
            ship.rightRotate();
            break;
        case(Keyboard::Key::Space):
            ship.emergencyStop();
            break;
        default:
            break;
    }
}

void updateStars()
{
    static Texture starTex("res/star.png");
    RectangleShape starDefault({150, 150});
    starDefault.setTexture(&starTex);
    for(int i = 0; i < STARSAMOUNT - stars.size(); i++)
    {
        stars.push_back(starDefault);
        float angle = rand()%360 * PI/180.f;
        vec2 pos = ship.getPos() + vec2(cos(angle) * (window.getSize().x + rand()%300), sin(angle) * (window.getSize().x + rand()%300));
        stars[stars.size()-1].setPosition(pos);
    }
    int i = 0;
    vector<int> toDelete;
    for(auto& star : stars)
    {
        window.draw(star);
        vec2 diff = ship.getPos() - star.getPosition();
        float dist = hypot(diff.x, diff.y);
        if(dist > sqrt(window.getSize().x * window.getSize().x) * 2.f) toDelete.push_back(i);
        i++;
    }
    for(auto& i : toDelete)
        stars.erase(stars.begin() + i);
}

