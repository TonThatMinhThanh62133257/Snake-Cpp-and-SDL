#include <iostream>
#include "../include/Entity.hpp"

Snake::Snake(int _x, int _y)
{
    x = _x;
    y = _y;
    next = NULL;
    prev = NULL;
}
