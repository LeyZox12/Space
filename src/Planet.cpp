#include "Planet.h"
#include "Element.h"
#include "Item.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "ext.hpp"
#include <chrono>
#include <thread>
#include <vector>

struct ContactContext;

Planet::Planet(vec2 pos, float radius) : radius(radius), ready(std::make_unique<std::atomic<int>>(0))
{
  gridSize = radius * 4;
  texture.resize(vec2u(gridSize, gridSize));
  bg.setSize(vec2(gridSize * pixelSize, gridSize * pixelSize));
  bg.setPosition(pos - bg.getSize() / 2.f);
  cellsInstances = std::vector<Element>(gridSize * gridSize, er.getElementById(AIR));
  cells = std::vector<uint8_t>(gridSize * gridSize * 4, 0);
  chunkSleeping = std::vector<bool>((gridSize/chunkSize) * (gridSize/chunkSize), false);
  for (int i = 0; i < gridSize * gridSize; i++) {
    cells[3 + i * 4] = 0;
  }
}


void Planet::executeOnGrid(std::function<void(int, int, Planet&)> task)
{
  for(int i = 10; i < gridSize-10; i++)
  {
    for(int j = 10; j < gridSize-10; j++)
    {
      task(j,i, *this);
    }
  }
}

sf::Texture& Planet::getTexture()
{
  return texture;
}

float Planet::getRadius()
{
  return radius;
}

vec2 Planet::getPos()
{
  return bg.getPosition() + bg.getSize() / 2.f;
}

sf::RectangleShape Planet::getSprite()
{
  return bg;
}

int Planet::getGridSize() { return gridSize; }

void Planet::display(sf::RenderTarget &window) 
{ 
  texture.update(cells.data());
  bg.setTexture(&texture);
  window.draw(bg); 
}

void Planet::step() {
  return;
  clock_t start = clock();
  std::vector<uint8_t> buffer = cells;
  std::vector<Element> instanceBuffer = cellsInstances;
  std::vector<int> xs = std::vector<int>(gridSize);
  std::vector<int> ys = std::vector<int>(gridSize);

  for(auto& pixel : pendingPixels)
  {
    setPixel(pixel.pos.x, pixel.pos.y, pixel.e, buffer, instanceBuffer);
  }

  pendingPixels.clear();

  for (int i = 0; i < chunkSize; i++) {
    xs[i] = i;
  }

  ys = xs;

  shuffle(xs);
  shuffle(ys);
      for (int i = 0; i < gridSize * gridSize; i++) {
        int x, y;
        x = xs[i % gridSize];
        y = ys[i / gridSize];
        Element& e = getElement(x, y, instanceBuffer );
        if(e.awake > 0){
          e.awake--;
          if (e.hasPhysics) {
            updatePhysics(x, y, buffer, instanceBuffer, e);
          }
          /*
          for (int i = 0; i < e.getContactFunctionCount(); i++) {
            auto f = e.getContactFunction(i);
            executeOnAxis(
                [this, &buffer, x, y, f, &instanceBuffer ](int xw, int yw) {
                  f(ContactContext(*this,
                                  getElement(xw, yw, instanceBuffer),
                                  buffer, instanceBuffer, er, x, y, xw, yw));
                },
                x, y);
          }*/
        }
  }
  cells = buffer;
  cellsInstances = instanceBuffer;
}

void Planet::updateTexture()
{
}

vec2 Planet::getCellPos(float x, float y)
{
  float ratio = pixelSize;
  int fx = floor((x-bg.getPosition().x)/ratio);
  int fy = floor((y-bg.getPosition().y)/ratio); 
  return vec2(fx, fy);
}

void shiftColor(glm::vec4& color)
{
    float shiftRatio =1.0 +  (rand()%100 / 400.f);
    float grayScale = (0.2126f * (float)color.r+
                      0.7152 * (float)color.g + 
                      0.0722 * (float)color.b) * shiftRatio;

    int r = color.r * shiftRatio;
    int g = color.g * shiftRatio;
    int b = color.b * shiftRatio;

    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    color = glm::vec4(r, g, b, color.a);
}

void Planet::setPixel(int x, int y, Element e)
{
  int index = x * 4 + y * gridSize * 4;
  if (index < 0 || index >= gridSize * gridSize * 4)
    return;
  shiftColor(e.color);
  glm::vec4 value = e.color;
  cells[index] = value.r;
  cells[index + 1] = value.g;
  cells[index + 2] = value.b;
  cells[index + 3] = value.a;
  cellsInstances[x + y * gridSize] = e;

}

void Planet::executeOnAxis(std::function<void(int, int)> task, int x, int y) {
  task(x + 1, y + 1);
  task(x + 1, y - 1);
  task(x + 1, y);
  task(x - 1, y + 1);
  task(x - 1, y - 1);
  task(x - 1, y);
  task(x, y);
  task(x, y + 1);
}

void Planet::updatePhysics(int x, int y, std::vector<uint8_t> &buffer, std::vector<Element>& instanceBuffer ,
                           Element element) {
  vec2 center = vec2(gridSize / 2.f, gridSize / 2.f);
  vec2 diff = center - vec2(x, y);
  float dist = hypot(diff.x, diff.y);
  std::vector<vec2> moves;
  for (int i = 0; i < 8; i++) {
    if (getAirPixel(x + possibleMoves[i].x, y + possibleMoves[i].y, buffer) || 
        isLighterPixel(element, x+possibleMoves[i].x, y+possibleMoves[i].y, instanceBuffer ))
      moves.push_back(possibleMoves[i]);
    else
      moves.push_back(vec2(0, 0));
  }
  if (dist > 0.f)
    diff /= dist;
  std::vector<Move> dots;
  for (int i = 0; i < moves.size(); i++) {
    vec2 diff2 = moves[i];
    float dist2 = hypot(diff2.x, diff2.y);
    if (dist2 > 0.0)
      diff2 /= dist2;
    dots.push_back(Move(moves[i], (dot(diff, diff2) + 1.f) / 2.f));
  }

  float total = 0.f;
  for (auto &v : dots)
    total += v.chance;
  for (auto &v : dots)
    v.chance = v.chance / total * 100.f;
  int percent = rand() % 100;
  sort(dots.begin(), dots.end(),
       [](auto &e1, auto &e2) { return e1.chance < e2.chance; });
  int index = 0;
  // cout << "dots\n";
  // for(auto& v : dots) cout << v.chance<<"->(" << v.move.x << "," << v.move.y
  // << ")" << endl;
  //
  //  cout << endl;
  for (int i = 0; i < moves.size(); i++) {
    if (i == moves.size() - 1)
      index = i;
    else if (percent < dots[i].chance) {
      index = i;
      break;
    }
  }
  // index = moves.size()-1;
  if (moves.size() > 0 && dots[index].chance > element.fom) {
    vec2 move = dots[index].move;
    Element e2 = getElement(x+move.x, y+move.y, instanceBuffer);
    e2.awake = 2;
    element.awake = 2;
    setPixel(x, y, e2, buffer, instanceBuffer);
    setPixel(x + move.x, y + move.y, element, buffer, instanceBuffer );
  }
}

int Planet::getNeighbours(int x, int y) {
  int count = 0;
  if (getPixel(x + 1, y) != Item::AIR)
    count++;
  if (getPixel(x + 1, y + 1) != Item::AIR)
    count++;
  if (getPixel(x + 1, y - 1) != Item::AIR)
    count++;
  if (getPixel(x - 1, y + 1) != Item::AIR)
    count++;
  if (getPixel(x - 1, y - 1) != Item::AIR)
    count++;
  if (getPixel(x - 1, y) != Item::AIR)
    count++;
  if (getPixel(x, y - 1) != Item::AIR)
    count++;
  if (getPixel(x, y + 1) != Item::AIR)
    count++;
  return count;
}

sf::Color Planet::getPixel(int x, int y) {
  int index = x * 4 + y * gridSize * 4;
  if (index < 0 || index >= gridSize * gridSize * 4)
    return sf::Color::Black;
  return sf::Color(cells[index], cells[index + 1], cells[index + 2],
               cells[index + 3]);
}

sf::Color Planet::getPixel(int x, int y, std::vector<uint8_t> &buffer) {
  int index = x * 4 + y * gridSize * 4;
  if (index < 0 || index >= gridSize * gridSize * 4)
    return sf::Color::Black;
  return sf::Color(buffer[index], buffer[index + 1], buffer[index + 2],
               buffer[index + 3]);
}

Element& Planet::getElement(int x, int y, std::vector<Element>& instanceBuffer)
{
  return instanceBuffer[x + y * gridSize];
}

Element& Planet::getElementAtfPos(float x, float y)
{
  float ratio = pixelSize;
  int fx = floor((x-bg.getPosition().x)/ratio);
  int fy = floor((y-bg.getPosition().y)/ratio); 
  
  if(fx < 0 || fy < 0 || fx >= gridSize || fy >= gridSize) return er.elements[0];
  return cellsInstances[fx + fy*gridSize];
}

int Planet::getPixelId(int x, int y, std::vector<Element>& instanceBuffer )
{
  return instanceBuffer[x + y * gridSize].id;
}

void Planet::setPixel(int x, int y, Element e, std::vector<uint8_t> &buffer, std::vector<Element>& instanceBuffer ) {
  int index = x * 4 + y * gridSize * 4;
  if (index < 0 || index >= gridSize * gridSize * 4)
    return;
  glm::vec4 value = e.color;
  buffer[index] = value.r;
  buffer[index + 1] = value.g;
  buffer[index + 2] = value.b;
  buffer[index + 3] = value.a;
  e.awake = 2;
  executeOnAxis([this, &instanceBuffer](int x, int y){instanceBuffer[x + y * gridSize].awake = 2;},x, y);
  instanceBuffer[x + y * gridSize] = e;
}


bool Planet::getSandPixel(int x, int y) { return getPixel(x, y) == Item::SAND; }

bool Planet::getWaterPixel(int x, int y) {
  return getPixel(x, y) == Item::WATER;
}

bool Planet::getAirPixel(int x, int y, std::vector<uint8_t> &buffer) {
  if (y == gridSize - 1 || x == 0 || x == gridSize - 1)
    return false;
  return getPixel(x, y) == Item::AIR &&
         getPixel(x, y, buffer) == Item::AIR;
}

bool Planet::isLighterPixel(Element e, int x, int y, std::vector<Element>& instanceBuffer )
{
  if (y == gridSize - 1 || x == 0 || x == gridSize - 1)
    return false;
  return getElement(x, y, instanceBuffer).weight < e.weight;
}