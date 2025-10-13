#include "Planet.h"
#include "Element.h"
#include "Item.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include <vector>

struct ContactContext;

Planet::Planet(vec2 pos, float radius, float pixelSize, ElementRegistry &er) : radius(radius), er(er) {

  gridSize = radius * 4;
  texture = sf::Texture(vec2u(gridSize, gridSize));
  bg.setSize(vec2(gridSize, gridSize));
  bg.setPosition(pos - bg.getSize() / 2.f);
  bg.setScale({4.f, 4.f});
  cellsInstances = std::vector<Element>(gridSize * gridSize, er.getElementById(AIR));
  cells = std::vector<uint8_t>(gridSize * gridSize * 4, 0);
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

float Planet::getRadius()
{
  return radius;
}

vec2 Planet::getPos()
{
  return bg.getPosition() + bg.getSize() / 2.f;
}

int Planet::getGridSize() { return gridSize; }

void Planet::display(sf::RenderTarget &window) 
{ 
  window.draw(bg); 
}

void Planet::step(Camera cam) {
  clock_t start = clock();
  std::vector<uint8_t> buffer = cells;
  std::vector<Element> instanceBuffer = cellsInstances;
  std::vector<int> xs = std::vector<int>(gridSize);
  std::vector<int> ys = std::vector<int>(gridSize);

  for (int i = 0; i < gridSize; i++) {
    xs[i] = i;
  }

  ys = xs;

  shuffle(xs);
  shuffle(ys);
#pragma omp parrallel for
  for (int i = 0; i < gridSize * gridSize; i++) {
    int x, y;
    x = xs[i % gridSize];
    y = ys[i / gridSize];
    Element e = getElement(x, y, instanceBuffer );
    if(e.awake > 0){
      e.awake--;
      if (e.getHasPhysics()) {
        updatePhysics(x, y, buffer, instanceBuffer, e);
      }
      for (int i = 0; i < e.getContactFunctionCount(); i++) {
        auto f = e.getContactFunction(i);
        executeOnAxis(
            [this, &buffer, x, y, f, &instanceBuffer ](int xw, int yw) {
              f(ContactContext(*this,
                              getElement(xw, yw, instanceBuffer),
                              buffer, instanceBuffer, er, x, y, xw, yw));
            },
            x, y);
      }
    }
  }
  cells = buffer;
  cellsInstances = instanceBuffer;
  texture.update(cells.data());
  bg.setTexture(&texture);
}

void Planet::updateTexture()
{
}

void Planet::setPixel(int x, int y, Element e)
{
  e.shiftColor();
  int index = x * 4 + y * gridSize * 4;
  sf::Color col = e.getColor();
  cells[index] = col.r;
  cells[index+1] = col.g;
  cells[index+2] = col.b;
  cells[index+3] = col.a;
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
  if (moves.size() > 0 && dots[index].chance > element.getFom()) {
    vec2 move = dots[index].move;
    setPixel(x, y, getElement(x+move.x, y+move.y, instanceBuffer), buffer, instanceBuffer);
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

Element Planet::getElement(int x, int y, std::vector<Element>& instanceBuffer)
{
  return instanceBuffer[x + y * gridSize];
}

int Planet::getPixelId(int x, int y, std::vector<Element>& instanceBuffer )
{
  return instanceBuffer[x + y * gridSize].getId();
}

void Planet::setPixel(int x, int y, Element e, std::vector<uint8_t> &buffer, std::vector<Element>& instanceBuffer ) {
  int index = x * 4 + y * gridSize * 4;
  if (index < 0 || index >= gridSize * gridSize * 4)
    return;
  sf::Color value = e.getColor();
  buffer[index] = value.r;
  buffer[index + 1] = value.g;
  buffer[index + 2] = value.b;
  buffer[index + 3] = value.a;
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
  return getElement(x, y, instanceBuffer).getWeight() < e.getWeight();
}