#pragma once
#include <iostream>
#include <SDL2/SDL.h>

class Snake
{
    public:
        int x, y;
        Snake(int _x, int _y);
        Snake* next;
        Snake* prev;
};
