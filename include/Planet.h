#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Element.h"
#include "ElementRegistry.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include <cmath>
#include "Item.hpp"
#include "ContactContext.hpp"
#include "Camera.hpp"
#include <functional>
#include "ElementRequest.hpp"
#include <thread>
#include "globals.hpp"
#include <atomic>

typedef sf::Vector2f vec2;
typedef sf::Vector2u vec2u;




class Planet
{
    public:
        Planet(vec2 pos, float radius);
        void step();
        void executeOnAxis(std::function<void(int, int)> task, int x, int y);
        void display(sf::RenderTarget& window);
        void updatePhysics(int x, int y, std::vector<uint8_t>& buffer, std::vector<Element>& instanceBuffer, Element element);
        int getNeighbours(int x, int y);
        sf::Color getPixel(int x, int y);
        float getRadius();
        void updateTexture();
        sf::Color getPixel(int x, int y, std::vector<uint8_t>& buffer);
        void setPixel(int x, int y, Element e, std::vector<uint8_t>& buffer, std::vector<Element>& instanceBuffer );
        void setPixel(int x, int y, Element e);
        Element& getElement(int x, int y, std::vector<Element>& instanceBuffer);
        int getPixelId(int x, int y, std::vector<Element>& instanceBuffer);
        vec2 getCellPos(float x, float y);
        unsigned int getGridSize();
        bool getSandPixel(int x, int y);
        bool getWaterPixel(int x, int y);
        vec2 getPos();
        sf::Texture& getTexture();
        void executeOnGrid(std::function<void(int, int, Planet&)> task);
        bool getAirPixel(int x, int y, std::vector<uint8_t>& buffer);
        Element& getElementAtfPos(float x, float y);
        bool isLighterPixel(Element e, int x, int y, std::vector<Element>& instanceBufferinstanceBuffer );
        bool doneUpdating = true;
        sf::RectangleShape getSprite();
        std::vector<Element> cellsInstances;
        std::vector<uint8_t> cells;
    private:
        const int chunkSize = 256;
        unsigned int gridSize = 0;
        float radius;

        std::unique_ptr<std::atomic<int>> ready = 0;
        sf::Texture texture;
        sf::RectangleShape bg;
        std::vector<bool> chunkSleeping;
        std::vector<ElementRequest> pendingPixels;
        struct Move
        {
            Move(vec2 move, float chance):move(move), chance(chance){}
            float chance;
            vec2 move;
        };
        float dot(vec2 v1, vec2 v2)
        {
            return v1.x * v2.x + v2.y * v1.y;
        }

        void shuffle(std::vector<int>& arr)
        {
            for(int i = arr.size()-1; i>0; i--)
            {
                int randomI = rand()%arr.size();
                int buffer = arr[randomI];
                arr[randomI] = arr[i];
                arr[i] = buffer;
            }
        }
        const vec2 possibleMoves[8] = 
        {
            vec2(-1, 1),
            vec2(0, 1),
            vec2(1, 1),
            vec2(-1, -1),
            vec2(0, -1),
            vec2(1, -1),
            vec2(1, 0),
            vec2(-1, 0),
        };
};
