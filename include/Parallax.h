#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Parallax
{
    public:
        Parallax();
        void addOnTop(std::string texturePath, float ratio, float scale);
        void update(sf::Vector2f pos);
        void display(sf::RenderTarget& window);
    private:
        std::vector<sf::Texture> textures;
        std::vector<sf::RectangleShape> elements;
        std::vector<float> ratios;
};