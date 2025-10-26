#pragma once
#include "ElementRegistry.h"
#include "ThreadPool.h"

inline const float pixelSize = 5.f;
inline ElementRegistry er;
inline ThreadPool threadPool(12);
inline const float SIMULATION_DISTANCE = 400.f;

struct VisualDebugger
{
    VisualDebugger()
    :rt(sf::RenderTexture(sf::Vector2u(1000, 1000)))
    {
        
    }
    void debug(sf::RectangleShape e)
    {
        rt.draw(e);
    }
    void draw(sf::RenderWindow& window)
    {
        sf::RectangleShape bg(vec2(rt.getSize().x, rt.getSize().y));
        rt.display();
        bg.setTexture(&rt.getTexture());
        window.draw(bg);
        rt.clear(sf::Color::Black);
    }
    sf::RenderTexture rt;
};

inline VisualDebugger debugger;

inline void init()
{
}