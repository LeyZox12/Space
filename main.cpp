#include "ElementRegistry.h"
#include "include/Element.h"
#include "include/Item.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
#define PI 3.141592653
#include "Camera.hpp"
#include "Map.h"
#include "Planet.h"
#include "Player.h"
#include "PointEngine.h"
#include "Ship.h"

using namespace sf;
using namespace std;

typedef Vector2f vec2;

enum Mode { SHIP, PLAYER };

// TODO PLANET COLLISION W/ SHIP
RenderWindow window(VideoMode({512, 512}), "space is cool");
Ship ship;
vector<Keyboard::Key> pressedKeys;
sf::Clock deltaClock;
Camera cam(vec2(window.getSize().x, window.getSize().y), 4.f);
vector<RectangleShape> stars;
RectangleShape bg;
vector<Planet> planets;
vector<thread> threads;
RenderTexture windowTex;
Map map(vec2(window.getSize().x - 300, window.getSize().y),
        vec2(window.getSize().x - 300, window.getSize().y - 300),
        vec2(300, 300));
Shader postProcessing;
PointEngine pe;

int mode = Mode::SHIP;
int shipPointIndex = 0;
bool holding = false;

const int STARSAMOUNT = 100;
const float pixelSize = 5.f;
const float PLAYER_SPEED = 5.f;
const float SEGMENT_SIZE = 10.f;
const float GRAVITY_MULT = 988.0f;

void start();
void inputManager(float dt, Player &player);
void incrementRope(Player &player);
void addPlanet(vec2 pos, float radius);

ElementRegistry er;

int main() {
  start();
  Player player(&pe.getPoint(1));
  while (window.isOpen()) {
    vec2 mousepos =
        vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
    vec2 mouseposWorld =
        window.mapPixelToCoords(Vector2i(mousepos.x, mousepos.y));
    float dt = 1.f / 60.f;
    while (optional<Event> e = window.pollEvent()) {
      if (e->is<Event::Closed>())
        window.close();
      if (e->is<Event::Resized>()) {
        Vector2u size = e->getIf<Event::Resized>()->size;
        View v({0, 0}, vec2(size.x, size.y));
        window.setView(v);
        cam.setSize(vec2(size.x, size.y));
        stars.clear();
        map.setPos(vec2(window.getSize().x - 300, window.getSize().y),
                   vec2(window.getSize().x - 300, window.getSize().y - 300));
        windowTex.resize({window.getSize().x, window.getSize().y});
        bg.setSize(vec2(window.getSize().x, window.getSize().y));
      }
      if (e->is<Event::KeyPressed>()) {
        Keyboard::Key key = e->getIf<Event::KeyPressed>()->code;
        if (count(pressedKeys.begin(), pressedKeys.end(), key) == 0)
          pressedKeys.push_back(key);
      } else if (e->is<Event::KeyReleased>()) {
        Keyboard::Key key = e->getIf<Event::KeyReleased>()->code;
        if (count(pressedKeys.begin(), pressedKeys.end(), key) == 1)
          pressedKeys.erase(find(pressedKeys.begin(), pressedKeys.end(), key));
        if (key == Keyboard::Key::F3)
          ship.toggleAdvancedDebug();
        else if (key == Keyboard::Key::LShift)
          ship.setThrottle(0.f);
        else if (key == Keyboard::Key::Tab)
          map.toggle();
        else if (key == Keyboard::Key::E) {
          mode == 0 ? mode = 1 : mode = 0;
          player.setVel(vec2(0, 0));
          for (int i = 0; i < pe.getPointCount(); i++) {
            Point &p = pe.getPoint(i);
            p.setPos(ship.getPos(), true);
            p.setOldPos(ship.getPos());
          }
          // if(mode == Mode::PLAYER) player.setPos(ship.getPos());
        }
      } else if (e->is<Event::MouseButtonPressed>()) {
        Mouse::Button button = e->getIf<Event::MouseButtonPressed>()->button;
        switch (button) {
        case (Mouse::Button::Left):

          if (!map.onClick(mouseposWorld)) {
            holding = true;
            incrementRope(player);
          }
          break;
        default:
          break;
        }
      } else if (e->is<Event::MouseButtonReleased>()) {
        holding = false;
      } else if (e->is<Event::MouseWheelScrolled>()) {
        map.onScroll(e->getIf<Event::MouseWheelScrolled>()->delta);
      }
    }
    pe.getPoint(shipPointIndex).setPos(ship.getPos(), true);
    if (holding)
      incrementRope(player);
    inputManager(dt, player);
    vec2 target = mode == Mode::SHIP ? ship.getPos() : player.getPos();
    cam.update(target);
    bg.setPosition(cam.getPos() - cam.getSize() * 0.5f);
    windowTex.setView(cam.getView());
    window.setView(cam.getView());
    static clock_t start = clock();
    if(clock() - start > 100)
    {
      for(auto& planet: planets) planet.step(cam);
      start = clock();
    }
    map.update(window, ship.getPos(), dt);
    ship.update(planets, dt);
    window.clear(Color::Black);
    windowTex.clear();
    ship.draw(windowTex);
    if (mode == Mode::PLAYER) {
      player.update(planets[ship.getCurrentPlanet()], mouseposWorld,
                    ship.getLanded(), dt);
      pe.getPoint(0).setPos(ship.getPos(), true);
      pe.updatePointPos(dt, mouseposWorld);
      pe.applyConstraints(4, dt);
      pe.applyCollisions(4);
      pe.displayAsRects(windowTex, Color::White, 10.f);
      player.draw(windowTex);
    }
    windowTex.display();
    postProcessing.setUniform("screen", windowTex.getTexture());
    postProcessing.setUniform("pixelSize", pixelSize);
    postProcessing.setUniform("u_resolution",
                              vec2(window.getSize().x, window.getSize().y));
    window.draw(bg, &postProcessing);
    map.draw(window, planets, mouseposWorld, ship.getPos());
    ship.debugOnScreen(window, dt);
    // pe.display(window, Color::Blue);
    for (auto &planet : planets)
      planet.display(window);
    window.display();
  }
}

void start() {
  ship.setPos(vec2(256, 256));
  window.setFramerateLimit(60);
  windowTex.resize({window.getSize().x, window.getSize().y});
  bg.setSize(vec2(window.getSize().x, window.getSize().y));
  if (!postProcessing.loadFromFile("res/postProcessing.frag",
                                   Shader::Type::Fragment))
    cout << "couldn't load shader";
  pe.addPoint(vec2(0, 0), false, false, SEGMENT_SIZE * 0.5f, 0);
  pe.addPoint(ship.getPos(), false, false, SEGMENT_SIZE * 0.5f, 0);
  int i1 = pe.getPointCount() - 1;
  int i2 = pe.getPointCount() - 2;
  shipPointIndex = i2;
  pe.getPoint(i1).setGravityScale(0.f);
  pe.getPoint(i2).setGravityScale(0.f);
  pe.addConstraint(i1, i2, PointEngine::DISTANCE_CONSTRAINT_MINMAX, SEGMENT_SIZE);
    er.registerElement(Element(
       "Air",
       0,
       false,
       0.0,
       Item::AIR
    ));
    
    er.registerElement(Element(
       "Sand",
       2,
       true,
       19.0,
       Item::SAND
    ));

    er.registerElement(Element(
       "Water",
       1,
       true,
       1.0,
       Item::WATER
    ));


    er.registerElement(Element(
       "Lava",
       1,
       true,
       12.5,
       Item::LAVA,
       {
        //Contact with water
        [](ContactContext ctx)
        {
            if(ctx.contact.getName() == "Water"){
                ctx.planet.setPixel(ctx.selfX, ctx.selfY, ctx.er.getElementById(STONE), ctx.buffer, ctx.idBuffer);
                ctx.planet.setPixel(ctx.contactX, ctx.contactY, ctx.er.getElementById(STONE), ctx.buffer, ctx.idBuffer);
            }
        }
       }
    ));

    er.registerElement(Element(
       "Stone",
       10,
       false,
       1.0,
       Item::STONE
    ));

    er.registerElement(Element(
       "Virus",
       1,
       false,
       0.f,
       Item::VIRUS,
       {
        [](ContactContext ctx)
        {
            if(!ctx.planet.getAirPixel(ctx.contactX, ctx.contactY,ctx.buffer) && 
                ctx.planet.getPixel(ctx.contactX, ctx.contactY) != Item::VIRUS &&
                ctx.planet.getPixel(ctx.contactX, ctx.contactY, ctx.buffer) != Item::VIRUS)
                ctx.planet.setPixel(ctx.contactX, ctx.contactY, ctx.er.getElementById(VIRUS), ctx.buffer, ctx.idBuffer);
        }
       }
    ));
    
  addPlanet(vec2(600, 200), 60.f);
  addPlanet(vec2(-600, 200), 60.f);
  //addPlanet(vec2(1300, -2000), 100.f);
  //addPlanet(vec2(-3000, 2000), 600.f);
  //addPlanet(vec2(100200, 0), 10000.f);
}

void inputManager(float dt, Player &player) {
  switch (mode) {
  case (Mode::SHIP):
    for (auto &key : pressedKeys)
      switch (key) {
      case (Keyboard::Key::Up):
        cam.getView().zoom(1.1f);
        break;
      case (Keyboard::Key::Down):
        cam.getView().zoom(0.9f);
        break;
      case (Keyboard::Key::Z):
        ship.accelerate(dt);
        break;
      case (Keyboard::Key::S):
        ship.deccelerate(dt);
        break;
      case (Keyboard::Key::Q):
        ship.leftRotate();
        break;
      case (Keyboard::Key::D):
        ship.rightRotate();
        break;
      case (Keyboard::Key::Space):
        ship.emergencyStop();
        break;
      default:
        break;
      }
    break;
  case (Mode::PLAYER): {
    float angle = player.getRot();
    vec2 translation =
        vec2(cos(angle) * PLAYER_SPEED, sin(angle) * PLAYER_SPEED);
    if (ship.getLanded())
      for (auto &key : pressedKeys)
        switch (key) {
        case (Keyboard::Key::Z):

          break;
        case (Keyboard::Key::S):
          break;
        case (Keyboard::Key::Q):
          player.move(-translation);
          break;
        case (Keyboard::Key::D):
          player.move(translation);
          break;
        case (Keyboard::Key::Space):
          break;
        default:
          break;
        }
    else {
      for (auto &key : pressedKeys)
        switch (key) {
        case (Keyboard::Key::Z):
          player.move({0, -PLAYER_SPEED});
          break;
        case (Keyboard::Key::S):
          player.move({0, PLAYER_SPEED});
          break;
        case (Keyboard::Key::Q):
          player.move({PLAYER_SPEED, 0});
          break;
        case (Keyboard::Key::D):
          player.move({-PLAYER_SPEED, 0});
          break;
        case (Keyboard::Key::Space):
          break;
        default:
          break;
        }
    }
  }
  }
}

void incrementRope(Player &player) {

  pe.addPoint(ship.getPos() + vec2(SEGMENT_SIZE, 0), false, false,
              SEGMENT_SIZE * 0.5f, 0);
  Point &p = pe.getPoint(pe.getPointCount() - 1);
  p.setGravityScale(0.f);

  pe.addConstraint(pe.getPointCount() - 2, pe.getPointCount() - 1,
                   PointEngine::DISTANCE_CONSTRAINT_MINMAX, SEGMENT_SIZE);
  player.setPoint(&p);
}

void addPlanet(vec2 pos, float radius) {
  Planet p (pos, radius, pixelSize, er);
  p.executeOnGrid([](int x, int y, Planet& p)
  {
    Element e = er.getElementById(LAVA);
    if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()/10.f) p.setPixel(x, y, er.getElementById(ITEMID::LAVA));
    else if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()/4.f) p.setPixel(x, y, er.getElementById(ITEMID::STONE));
    else if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()) p.setPixel(x, y, er.getElementById(ITEMID::SAND));
  });
  p.step(cam);
  planets.emplace_back(p);

}
