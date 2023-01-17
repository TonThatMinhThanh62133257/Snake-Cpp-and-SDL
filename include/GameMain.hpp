#pragma once
#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>

#include "Entity.hpp"

class Game
{
    public:
        Game(char* title, int width, int height);
        ~Game();
    private:
        int windowWidth;
        int windowHeight;
        SDL_Window* mainWindow;
        SDL_Renderer* mainRenderer;
        void Init();
        void Update();
        /*INSERT NEW VARIABLES OR FUNCTIONS HERE*/
        int boardSize;
        int** board;
        int snakeLength;
        int snakeDir;
        int food_x, food_y;
        Snake* head;
        Snake* tail;
        void CreateBoard();
        void PrintBoard();
        void CreateSnake();
        int AddHead(int direction);
        void RemoveTail();
        void RenderSnake();
        void CreateFood();
        void GameOver();
        void DestroySnake();
        //Snake* ReturnTail(Snake* head);
        //void MoveSnake();
        /*INSERT NEW VARIABLES OR FUNCTIONS HERE*/
};
