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
        void display(sf::RenderTarget& window);
        void executeOnGrid(std::function<void(int, int, Planet&)> task);
        void setPixel(int x, int y, Element e, bool shouldShiftColor = true);
        void visualDebug(sf::RenderTarget& window);
        void updatePhysics(int x, int y);
        void swap(int x1, int y1, int x2, int y2);
        int getIndexInChunk(int x, int y);
        int getGridSize();
        float getRadius();
        sf::RectangleShape& getSprite();
        Element& getPixel(int x, int y);
        Element& getPixelInBuffers(int x, int y);
        Element& getElementAtfPos(sf::Vector2f pos);
        sf::Vector2i getGridPos(sf::Vector2f pos);
    private:
        void processChunk(int cx, int cy);
        sf::Shader shadowsShader;
        sf::RectangleShape sprite;
        int chunkSize = 64;
        int chunkCount = 1;
        int gridSize;
        std::vector<Element> cells;
        std::vector<uint8_t> textureData;
        std::vector<std::vector<Element>> buffers;
        std::vector<std::mutex> chunkMutexes;
        sf::Texture texture;
        const sf::Vector2f possibleMoves[8] = 
        {
            sf::Vector2f(-1, 1),
            sf::Vector2f(0, 1),
            sf::Vector2f(1, 1),
            sf::Vector2f(-1, -1),
            sf::Vector2f(0, -1),
            sf::Vector2f(1, -1),
            sf::Vector2f(1, 0),
            sf::Vector2f(-1, 0),
        };
        
        float dot(vec2 v1, vec2 v2)
        {
            return v1.x * v2.x + v2.y * v1.y;
        }

        struct Move
        {
            Move(vec2 move, float chance):move(move), chance(chance){}
            float chance;
            vec2 move;
        };
};