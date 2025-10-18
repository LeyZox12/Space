#include <SFML/Graphics.hpp>
#include "Element.h"

struct ElementRequest
{
    ElementRequest(sf::Vector2u pos, Element e)
    :pos(pos), e(e){}
    sf::Vector2u pos;
    Element e;
};