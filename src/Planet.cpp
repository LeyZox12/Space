#include "Planet.h"
#include "Element.h"
#include "Item.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "globals.hpp"
#include <cmath>
#include <cstdint>
#include <mutex>
#include <thread>

Planet::Planet(sf::Vector2f pos, float radius)
:gridSize(std::round(radius*4)), chunkMutexes(std::vector<std::mutex>(ceil(gridSize / (float)chunkSize) * ceil(gridSize / (float)chunkSize)))
{  
  cells = std::vector<Element>(gridSize*gridSize, er.getElementById(AIR));
  textureData = std::vector<uint8_t>(gridSize*gridSize*4, 0);
  sprite.setSize({gridSize*pixelSize, gridSize*pixelSize});
  buffers = std::vector<std::vector<Element>>(ceil(gridSize/(float)chunkSize) * ceil(gridSize/(float)chunkSize), std::vector<Element>(chunkSize * chunkSize, er.getElementById(AIR)));
  if(!texture.resize(sf::Vector2u(gridSize, gridSize)))
  {
    std::cout << "error when creating texture" << std::endl;
  }
  if(!shadowsShader.loadFromFile("res/shaders/shadows.frag", sf::Shader::Type::Fragment))
    std::cout << "couldn't load planet render shaders (shadows.frag)\n";
  sprite.setPosition(pos - sprite.getSize() / 2.f);
  chunkCount = ceil(gridSize / (float)chunkSize) * ceil(gridSize / (float)chunkSize);
  std::cout << chunkCount << std::endl;
}

void shuffle(std::vector<int>& v)
{
  for(int i = 0; i < v.size(); i++)
  {
    int swapIndex = rand()%v.size();
    int buff = v[i];
    v[i] = v[swapIndex];
    v[swapIndex] = buff;
  }
}

void Planet::processChunk(int cx, int cy)
{
  //return;
  std::vector<int> xs = std::vector<int>(chunkSize);
  std::vector<int> ys = std::vector<int>(chunkSize);
  for (int i = 0; i < chunkSize; i++) {
    xs[i] = i;
  }
  for (int i = 0; i < chunkSize; i++) {
    ys[i] = i;
  }

  shuffle(xs);
  shuffle(ys);

  for(int c = 0; c < chunkSize * chunkSize; c++)
  {
    int x, y;
    x = cx * chunkSize + c%chunkSize;
    y = cy * chunkSize + c/chunkSize;
    buffers[cx + cy * ceil(gridSize/(float)chunkSize)][c] = getPixel(x, y);
  }
  for(int c = 0; c < chunkSize * chunkSize; c++)
  {
    int x, y;
    x = cx * chunkSize + xs[c%chunkSize];
    y = cy * chunkSize + ys[c/chunkSize];
    if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
      Element e = getPixelInBuffers(x, y);
      if(e.hasPhysics)
        updatePhysics(x, y);
    }
  }
  for(int c = 0; c < chunkSize * chunkSize; c++)
  {
    int x, y;
    x = cx * chunkSize + c%chunkSize;
    y = cy * chunkSize + c/chunkSize;
  }
}

void Planet::step(sf::Vector2f pos)
{
  for(int i = 0; i < chunkCount; i++)
  {
    int cx, cy;
    int chunkRow = ceil(gridSize / (float)chunkSize);
    cx = i % chunkRow;
    cy = i / chunkRow;
    
    sf::Vector2f chunkWorldPosCenter = sf::Vector2f(sprite.getPosition().x + cx * chunkSize * pixelSize,
                                                    sprite.getPosition().y + cy * chunkSize * pixelSize);
    chunkWorldPosCenter += sf::Vector2f(chunkSize * pixelSize * .5f, chunkSize * pixelSize * .5f);
    sf::Vector2f diff = chunkWorldPosCenter - pos;
    if(hypot(diff.x, diff.y) < SIMULATION_DISTANCE * pixelSize)
    {
      //processChunk(cx, cy);
      threadPool.addTask([this, cx, cy](){processChunk(cx, cy);});
    }
  }
  texture.update(textureData.data());
}

void Planet::display(sf::RenderTarget& window)
{
  shadowsShader.setUniform("texture", texture);
  shadowsShader.setUniform("u_resolution", sprite.getSize());
  shadowsShader.setUniform("pixelSize", pixelSize);
  shadowsShader.setUniform("pos", sprite.getPosition());
  shadowsShader.setUniform("topLeft", window.getView().getCenter() - (sf::Vector2f)window.getSize() / 2.f);
  sprite.setTexture(&texture);
  //window.draw(sprite, &shadowsShader);
  window.draw(sprite);
}

void Planet::executeOnGrid(std::function<void(int, int, Planet&)> task)
{
  for(int i = 0; i < gridSize * gridSize; i++)
  {
    int x, y;
    x = i % gridSize;
    y = i / gridSize;
    task(x, y, *this);
  }
}

void shiftColor(sf::Color& color)
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
    color = sf::Color(r, g, b, color.a);
}

void Planet::setPixel(int x, int y, Element e, bool shouldShiftColor)
{
  int index = x + y * gridSize;
  int cIndex = x * 4 + y * gridSize * 4;
  if(index >= 0 && index < cells.size()){
    if(shouldShiftColor)
      shiftColor(e.color);
    cells[index] = e;
    textureData[cIndex] =   (uint8_t)e.color.r;
    textureData[cIndex+1] = (uint8_t)e.color.g;
    textureData[cIndex+2] = (uint8_t)e.color.b;
    textureData[cIndex+3] = (uint8_t)e.color.a;
  }
}

void Planet::visualDebug(sf::RenderTarget& window)
{
  sf::VertexArray line(sf::PrimitiveType::LineStrip, 2);
  line[0].color = sf::Color::Yellow;
  line[1].color = sf::Color::Yellow;
  float ratio = sprite.getSize().x / ((float)chunkSize/pixelSize);
  for(int i = 0; i < ceil(gridSize/(float)chunkSize)+1; i++)
  {
    line[0].position = sf::Vector2f(sprite.getPosition().x + chunkSize*pixelSize * i, sprite.getPosition().y);
    line[1].position = sf::Vector2f(sprite.getPosition().x + chunkSize*pixelSize * i, sprite.getPosition().y+sprite.getSize().y);
    window.draw(line);
    line[0].position = sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y + chunkSize*pixelSize * i);
    line[1].position = sf::Vector2f(sprite.getPosition().x+sprite.getSize().x, sprite.getPosition().y + chunkSize*pixelSize * i);
    window.draw(line);
  }
}

void Planet::updatePhysics(int x, int y)
{
  Element& e = getPixelInBuffers(x, y);
  e.vel = vec2(gridSize / 2.f, gridSize / 2.f) - vec2(x, y);
  vec2 target = vec2(x + e.vel.x, y + e.vel.y);
  vec2 diff = target - vec2(x, y);
  float dist = hypot(diff.x, diff.y);
  std::vector<vec2> moves;
  for (int i = 0; i < 8; i++) {
    vec2 move = possibleMoves[i];
    Element& e2 = getPixelInBuffers(x+move.x, y+move.y);
    if(e2.weight < e.weight && getPixel(x+move.x, y+move.y).weight < e.weight)
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
  if (moves.size() > 0 && dots[index].chance > e.fom) {
    vec2 move = dots[index].move;
    swap(x, y, x+move.x, y+move.y);
  }
}

void Planet::swap(int x1, int y1, int x2, int y2)
{
  Element e = getPixelInBuffers(x1, y1);
  Element e2 = getPixelInBuffers(x2, y2);
  setPixel(x1, y1, e2, false);
  setPixel(x2, y2, e, false);
}

int Planet::getIndexInChunk(int x, int y)
{
  return x%chunkSize + y%chunkSize*chunkSize; 
}

int Planet::getGridSize()
{
  return gridSize;
}

float Planet::getRadius()
{
  return gridSize / 4.f;
}

sf::RectangleShape& Planet::getSprite()
{
  return sprite;
}

sf::Vector2i Planet::getGridPos(sf::Vector2f pos)
{
  return sf::Vector2i((pos.x-sprite.getPosition().x) / pixelSize, (pos.y-sprite.getPosition().y) / pixelSize);
}

Element& Planet::getPixel(int x, int y)
{
  int index = x + y * gridSize;
  if(index >= 0 && index < cells.size())
    return cells[index];
  return er.getElementById(ERR);
}

Element& Planet::getPixelInBuffers(int x, int y)
{
  int index = x + y * gridSize;
  int chunkIndex = x/chunkSize + y / chunkSize * ceil(gridSize/(float)chunkSize);
  if(index >= 0 && index < cells.size()){
    std::lock_guard<std::mutex> lock(chunkMutexes[chunkIndex]);
    return buffers[chunkIndex][getIndexInChunk(x, y)];
  }
}

Element& Planet::getElementAtfPos(sf::Vector2f pos)
{
  sf::Vector2i ipos = getGridPos(pos);
  int index = ipos.x + ipos.y * gridSize;
  return cells[index];
}