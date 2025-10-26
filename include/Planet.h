#pragma once
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include "Element.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "globals.hpp"

class Planet
{
    public:
        Planet(sf::Vector2f pos, float radius);
        void step(sf::Vector2f pos);
        void updatePhysics();
        void display(sf::RenderTarget& window);
        void executeOnGrid(std::function<void(int, int, Planet&)> task);
        void setPixel(int x, int y, Element e);
        void visualDebug(sf::RenderTarget& window);
        int getGridSize();
        float getRadius();
        sf::RectangleShape& getSprite();
        Element getPixel(int x, int y);
    private:
        void processChunk(int cx, int cy);
        sf::Shader shadowsShader;
        sf::RectangleShape sprite;
        int chunkSize = 64;
        int gridSize;
        std::vector<Element> cells;
        std::vector<uint8_t> textureData;
        sf::Texture texture;

};