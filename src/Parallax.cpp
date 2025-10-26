#include "Parallax.h"
#include "SFML/Graphics/Rect.hpp"

Parallax::Parallax()
{

}

void Parallax::addOnTop(std::string texturePath, float ratio, float scale)
{
    textures.push_back(sf::Texture());
    sf::RectangleShape rect;
    sf::Texture* t = &textures[textures.size()-1];
    if(!t->loadFromFile(texturePath))
    {
        std::cout << "couldn't load texture for " << texturePath << std::endl;
    }
    rect.setSize((sf::Vector2f)t->getSize());
    t->setRepeated(true);
    rect.setScale(sf::Vector2f(scale, scale));
    elements.push_back(rect);
    ratios.push_back(ratio);
}

void Parallax::update(sf::Vector2f pos)
{
    for(int i = 0; i < elements.size(); i++)
    {
        elements[i].setTexture(&textures[i]);
        elements[i].setTextureRect(sf::IntRect({(sf::Vector2i)(pos * ratios[i])},
                                   {(sf::Vector2i)textures[i].getSize()}));
    }
}

void Parallax::display(sf::RenderTarget& window)
{
    for(auto& e : elements)
    {
        e.setPosition(window.getView().getCenter() - window.getView().getSize() / 2.f);
        sf::IntRect rect = e.getTextureRect();
        e.setSize(window.getView().getSize());
        e.setTextureRect(sf::IntRect({rect.position}, {(sf::Vector2i)window.getView().getSize()}));
        window.draw(e);
    }
}