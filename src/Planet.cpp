#include "Planet.h"
#include "Item.hpp"
#include "globals.hpp"
#include <cstdint>
#include <thread>

Planet::Planet(sf::Vector2f pos, float radius)
{
  gridSize = std::round(radius * 4);
  cells = std::vector<Element>(gridSize*gridSize, er.getElementById(AIR));
  textureData = std::vector<uint8_t>(gridSize*gridSize*4, 0);
  sprite.setSize({gridSize*pixelSize, gridSize*pixelSize});
  if(!texture.resize(sf::Vector2u(gridSize, gridSize)))
  {
    std::cout << "error when creating texture" << std::endl;
  }
  sprite.setPosition(pos - sprite.getSize() / 2.f);
}

void Planet::processChunk(int cx, int cy)
{
    std::vector<Element> buffer(chunkSize*chunkSize);
    for(int c = 0; c < chunkSize * chunkSize; c++)
    {
      int x, y;
      x = cx * chunkSize + c%chunkSize;
      y = cy * chunkSize + c/chunkSize;
    }
    for(int c = 0; c < chunkSize * chunkSize; c++)
    {
      int x, y;
      x = cx * chunkSize + c%chunkSize;
      y = cy * chunkSize + c/chunkSize;
      if(getPixel(x, y).id != AIR)
        setPixel(x, y, er.getElementById(VIRUS));
    }
}

void Planet::step(sf::Vector2f pos)
{
  int chunkCount = gridSize * gridSize / chunkSize;
  for(int i = 0; i < chunkCount; i++)
  {
    int cx, cy;
    cx = i % chunkSize;
    cy = i / chunkSize;
    sf::Vector2f chunkWorldPosCenter = sf::Vector2f(sprite.getPosition().x + cx * chunkSize * pixelSize,
                                                    sprite.getPosition().y + cy * chunkSize * pixelSize);
    chunkWorldPosCenter += sf::Vector2f(chunkSize * pixelSize * .5f, chunkSize * pixelSize * .5f);
    sf::Vector2f diff = chunkWorldPosCenter - pos;
    if(hypot(diff.x, diff.y) < SIMULATION_DISTANCE * pixelSize)
    {
      threadPool.addTask([this, cx, cy](){processChunk(cx, cy);});
    }
  }
  texture.update(textureData.data());
}

void Planet::updatePhysics()
{

}

void Planet::display(sf::RenderTarget& window)
{
  sprite.setTexture(&texture);
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

void Planet::setPixel(int x, int y, Element e)
{
  e.vel = sf::Vector2f(1.f, 1.f);
  int index = x + y * gridSize;
  int cIndex = x * 4 + y * gridSize * 4;
  shiftColor(e.color);
  cells[index] = e;
  textureData[cIndex] = (uint8_t)e.color.r;
  textureData[cIndex+1] = (uint8_t)e.color.g;
  textureData[cIndex+2] = (uint8_t)e.color.b;
  textureData[cIndex+3] = (uint8_t)e.color.a;
}

void Planet::visualDebug(sf::RenderTarget& window)
{
  sf::VertexArray line(sf::PrimitiveType::LineStrip, 2);
  line[0].color = sf::Color::Yellow;
  line[1].color = sf::Color::Yellow;
  float ratio = sprite.getSize().x / ((float)chunkSize/pixelSize);
  for(int i = 0; i < gridSize/chunkSize; i++)
  {
    line[0].position = sf::Vector2f(sprite.getPosition().x + chunkSize*pixelSize * i, sprite.getPosition().y);
    line[1].position = sf::Vector2f(sprite.getPosition().x + chunkSize*pixelSize * i, sprite.getPosition().y+sprite.getSize().y);
    window.draw(line);
    line[0].position = sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y + chunkSize*pixelSize * i);
    line[1].position = sf::Vector2f(sprite.getPosition().x+sprite.getSize().x, sprite.getPosition().y + chunkSize*pixelSize * i);
    window.draw(line);
  }
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

Element Planet::getPixel(int x, int y)
{
  return cells[x + y * gridSize];
}