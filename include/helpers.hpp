#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "Element.h"
#include <functional>
#include <cmath>

typedef sf::Vector2f vec2;

struct CollisionCallbackContext
{
    CollisionCallbackContext(bool collided, vec2 hitPos)
    :collided(collided), hitPos(hitPos)
    {}
    bool collided;
    vec2 hitPos;
    vec2 spriteHitPos;
};


inline void checkCollision(sf::RectangleShape sprite, Planet& p, vec2 offset, std::function<void(CollisionCallbackContext)> callback)
{
    /*sf::Image bitmap = sprite.getTexture()->copyToImage();
    int size =std::max(sprite.getSize().y, sprite.getSize().x) / pixelSize ;
    for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++)
    {
        
        vec2 bitmaptoSpriteRatio = vec2(bitmap.getSize().x / sprite.getSize().x, bitmap.getSize().y / sprite.getSize().y);
        vec2 bitmapOrigin = vec2(sprite.getOrigin().x * bitmaptoSpriteRatio.x, sprite.getOrigin().y * bitmaptoSpriteRatio.y);
        vec2 pos = vec2(j, i) * pixelSize - vec2(sprite.getOrigin().x, sprite.getOrigin().y);

        vec2 ratio = vec2((bitmap.getSize().x*pixelSize) / sprite.getSize().x, (bitmap.getSize().y*pixelSize) /sprite.getSize().y);
        vec2 texPos = vec2(j * ratio.x, i * ratio.y);
        
        float angle = sprite.getRotation().asRadians();
        pos = vec2(pos.x * cos(angle) - pos.y * sin(angle),
                    pos.y * cos(angle) + pos.x * sin(angle));
        pos += sprite.getPosition();
        pos += offset;
        //pos = vec2(floor(pos.x/pixelSize)*pixelSize , floor((pos.y)/pixelSize)*pixelSize);
        if(texPos.x > 0.f && texPos.x < bitmap.getSize().x && texPos.y > 0.f && texPos.y < bitmap.getSize().y && bitmap.getPixel({texPos.x, texPos.y}) != Color(0, 0, 0, 0))
        {
            Element& e = p.getElementAtfPos(pos.x, pos.y);
            if(e.id > 1)
            {
                callback(CollisionCallbackContext(true, pos));
            }
            else
                callback(CollisionCallbackContext(false, pos));
        }
        //callback(CollisionCallbackContext(false, pos));
    }*/
}