#include <SFML/Graphics.hpp>
#pragma once

struct ContactContext;

namespace Item
{
    inline const sf::Color AIR = sf::Color(0, 0,0, 0);
    inline const sf::Color SAND = sf::Color(255, 230, 127, 255);
    inline const sf::Color WATER = sf::Color(112, 216, 250);
    inline const sf::Color LAVA = sf::Color(250, 106, 0);
    inline const sf::Color STONE = sf::Color(100, 100, 100);
    inline const sf::Color WOOD = sf::Color(91, 81, 61);
    inline const sf::Color GRASS = sf::Color(100, 152, 85);
    inline const sf::Color VIRUS = sf::Color(255, 100, 100);
    inline const sf::Color ICE = sf::Color(209, 248, 255);
    inline const sf::Color STEAM = sf::Color(38, 68, 130);
}

enum ITEMID
{
    ERR,
    AIR,
    SAND,
    WATER,
    LAVA,
    STONE,
    VIRUS,
    STEAM,
    FIRE,
    ICE,
    GUNPOWDER
};
