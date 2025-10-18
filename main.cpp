#include "ElementRegistry.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Window/Sensor.hpp"
#include "glad/glad.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "include/Element.h"
#include "include/Item.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
#define PI 3.141592653
#include "globals.hpp"
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
Shader collision;
PointEngine pe;

int mode = Mode::SHIP;
int shipPointIndex = 0;
bool holding = false;
bool shaderDisplay = false;

const int STARSAMOUNT = 100;
const float PLAYER_SPEED = 5.f;
const float SEGMENT_SIZE = 10.f;
const float GRAVITY_MULT = 988.0f;

const char* computeStepSrc =R"(
#version 460 core
layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;
struct Element{
    int awake;
    vec4 color;
    int id;
    int weight;
    int hasPhysics;
    float fom;
};
layout(std430, binding = 0) buffer planet{
    Element cells[];
};

layout(std430, binding = 1) buffer elementsbuff{
    Element elements[];
};

uniform uint gridSize;

uint getIndex(uint x, uint y)
{
    return x + gridSize * y;
}

uint isIsBounds(uint x, uint y)
{
  if(x > 0 && x < gridSize && y > 0 && y < gridSize) return 1;
  else
  return 0;
}

void main(){
    vec2 possibleMoves[8] = vec2[](
        vec2(-1, 1),
        vec2(0, 1),
        vec2(1, 1),
        vec2(-1, -1),
        vec2(0, -1),
        vec2(1, -1),
        vec2(1, 0),
        vec2(-1, 0)
    );
    uint x = gl_GlobalInvocationID.x;
    Element buff = cells[x];
    buff.color = vec4(255.0, 255.0, 255.0, 255.0);
    cells[x] = buff;
};


)";
GLuint stepShader;
GLuint stepProgram;
GLuint elementRegistrySSBO;
GLuint cellsSSBO;
GLuint colorsSSBO;

void start();
void inputManager(float dt, Player &player);
void incrementRope(Player &player);
void addPlanet(vec2 pos, float radius);
void initElements();

int main() {
  window.setActive(true);
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }
  start();
  Player player(&pe.getPoint(1));
  while (window.isOpen()) {
    vec2 mousepos =
        vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
    vec2 mouseposWorld =
        window.mapPixelToCoords(Vector2i(mousepos.x, mousepos.y));
    static float dt = 1.f;
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
        else if(key == Keyboard::Key::L)
        {
          ship.toggleLanding();
        }
        if(key == Keyboard::Key::Space)
          shaderDisplay = !shaderDisplay;
      } else if (e->is<Event::MouseButtonPressed>()) {
        Mouse::Button button = e->getIf<Event::MouseButtonPressed>()->button;
        switch (button) {
        case (Mouse::Button::Left):
          //cout << planets[0].getElementAtfPos(Mouse::getPosition(window).x, Mouse::getPosition(window).y).getName() << endl;;
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
      for(auto& p : planets){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, cellsSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cellsSSBO);
        size_t byteSizeCells = sizeof(Element) * p.getGridSize() * p.getGridSize();
        glBufferData(GL_SHADER_STORAGE_BUFFER, byteSizeCells, p.cellsInstances.data(), GL_DYNAMIC_DRAW);
        glUseProgram(stepProgram);
        glUniform1ui(glGetUniformLocation(stepProgram, "gridSize"), p.getGridSize());
        GLuint groupsX = (p.getGridSize() + 7) / 8;
        GLuint groupsY = ceil(p.getGridSize()*p.getGridSize() / 8.f);
        glDispatchCompute(groupsY, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, cellsSSBO);
        Element* ptsUpdated = (Element*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, byteSizeCells, GL_MAP_READ_BIT);
        p.cellsInstances = vector<Element>(ptsUpdated, ptsUpdated + byteSizeCells/sizeof(Element));
        for(int i = 0; i < p.getGridSize() * p.getGridSize(); i++)
        {
          Element e = p.cellsInstances[i];
          p.cells[i * 4] = e.color.r;
          p.cells[i * 4+1] = e.color.g;
          p.cells[i * 4+2] = e.color.b;
          p.cells[i * 4+3] = e.color.a;
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
      }
      start = clock();
    }
    windowTex.clear();
    map.update(window, ship.getPos(), dt);
    ship.update(planets, dt);
    window.clear(Color::Black);
    windowTex.clear();
    for (auto &planet : planets)
      planet.display(windowTex);
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
    //postProcessing.setUniform("pixelSize", pixelSize);
    postProcessing.setUniform("pixelSize", 1.f);
    postProcessing.setUniform("u_resolution",
                              vec2(window.getSize().x, window.getSize().y));
    window.draw(bg, &postProcessing);
    /*Texture t;
    t.update(img);
    bg.setTexture(&t);
    window.draw(bg);*/
    map.draw(window, planets, mouseposWorld, ship.getPos());
    ship.debugOnScreen(planets, window, dt);
    // pe.display(window, Color::Blue);*/
    window.display();
    dt= deltaClock.restart().asSeconds();
  }
  return 0;
}

void start() {
  initElements();

  glGenBuffers(1, &cellsSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, cellsSSBO);

  glGenBuffers(1, &colorsSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, colorsSSBO);

  glGenBuffers(1, &elementRegistrySSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, elementRegistrySSBO);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, elementRegistrySSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Element) * er.elements.size(), er.elements.data(), GL_DYNAMIC_DRAW);

  stepProgram = glCreateProgram();
  stepShader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(stepShader, 1, &computeStepSrc, NULL);
  glCompileShader(stepShader);
  GLint isCompiled = 0;
  glGetShaderiv(stepShader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(stepShader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(stepShader, maxLength, &maxLength, &errorLog[0]);
    for(auto& c : errorLog) cout << c;
    glDeleteShader(stepShader);
    return;
  };
  glAttachShader(stepProgram, stepShader);
  glLinkProgram(stepProgram);

  ship.setPos(vec2(256, 256));
  window.setFramerateLimit(60);
  windowTex.resize({window.getSize().x, window.getSize().y});
  bg.setSize(vec2(window.getSize().x, window.getSize().y));
  if (!postProcessing.loadFromFile("res/postProcessing.frag",
                                   Shader::Type::Fragment))
    cout << "couldn't load shader";
  if (!collision.loadFromFile("res/collision.frag",
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
  addPlanet(vec2(700, 200), 60.f);
  //addPlanet(vec2(-600, 200), 60.f);
  //addPlanet(vec2(0, -500), 100.f);
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
  planets.emplace_back(pos, radius);
  Planet& p = planets[planets.size()-1];
  p.executeOnGrid([](int x, int y, Planet& p)
  {
    Element e = er.getElementById(LAVA);
    if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()/10.f) p.setPixel(x, y, er.getElementById(ITEMID::LAVA));
    else if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()/1.5f) p.setPixel(x, y, er.getElementById(ITEMID::STONE));
    else if(hypot(p.getGridSize()/2.f - x, p.getGridSize()/2.f- y) < p.getRadius()) p.setPixel(x, y, (rand()%10 == 0 ? er.getElementById(ITEMID::SAND) : er.getElementById(ITEMID::SAND)));
  });
  int index = planets.size()-1;
  threads.emplace_back(thread([index](){
    while(true){
      this_thread::sleep_for(100ms);
      planets[index].step();
    }
  }));
}

void initElements()
{
    er.registerElement(
       "Air",
       0,
       false,
       0.0,
       Item::AIR
    );
    
    er.registerElement(
       "Sand",
       2,
       true,
       19.0,
       Item::SAND
    );

    er.registerElement(
       "Water",
       1,
       true,
       1.0,
       Item::WATER
    );


    er.registerElement(
       "Lava",
       1,
       true,
       12.5,
       Item::LAVA
    );

    er.registerElement(
       "Stone",
       10,
       false,
       1.0,
       Item::STONE
    );

    er.registerElement(
       "Virus",
       1,
       false,
       0.f,
       Item::VIRUS
    );
}