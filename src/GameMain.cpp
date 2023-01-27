#include <iostream>
#include <cstdlib>
#include <ctime>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../include/GameMain.hpp"
#include "../include/Entity.hpp"

#define CELLSIZE 10

enum SNAKE_DIRECTION {UP,DOWN,LEFT,RIGHT};
enum CELL_TYPE {FOOD = -1, DEFAULT = 0, SNAKE = 1};

Game::Game(char* title, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    if((SDL_Init(SDL_INIT_EVERYTHING)) < 0)
    {
        std::cout << "SDL failed to initialize: " << SDL_GetError() << std::endl;
        return;
    }
    if(TTF_Init() < 0)
    {
        std::cout << "TTF failed to initialize: " << TTF_GetError() << std::endl;
        return;
    }
    /*INSERT MORE INIT FUNCTIONS HERE*/
    if((mainWindow = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,windowWidth,windowHeight,SDL_WINDOW_SHOWN)) == NULL)
    {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return;
    }
    if((mainRenderer = SDL_CreateRenderer(mainWindow,-1,SDL_RENDERER_SOFTWARE)) == NULL)
    {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return;
    }
    if(Menu() == false)
        return;
    Init();
    Update();
}

Game::~Game()
{
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    std::cout << "Your score is " << snakeLength << std::endl;
}

bool Game::Menu()
{
    int quitMenu = 0;
    SDL_Event event;
    TTF_Font* font = NULL;
    if((font = TTF_OpenFont("font/terminal.ttf",14)) == NULL)
    {
        std::cout << "Cannot load font: " << TTF_GetError() << std::endl;
        return false;
    }
    SDL_Surface* titleText = TTF_RenderText_Solid(font,"SNAKE - PRESS ANY KEY TO PLAY", {255,255,255});
    if(titleText == NULL)
    {
        std::cout << "Cannot load title surface\n";
        return false;
    }
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(mainRenderer,titleText);
    if(titleTexture == NULL)
    {
        std::cout << "Cannot load title texture\n";
        return false;
    }
    int titleWidth = titleText->w, titleHeight = titleText->h;
    SDL_FreeSurface(titleText);
    SDL_SetRenderDrawColor(mainRenderer,0,0,0,255);
    SDL_RenderClear(mainRenderer);
    SDL_Rect titleRect = {windowWidth / 4, windowHeight / 2, titleWidth, titleHeight};
    SDL_RenderCopy(mainRenderer,titleTexture,NULL,&titleRect);
    SDL_RenderPresent(mainRenderer);
    while(quitMenu == 0)
        while(SDL_PollEvent(&event) != 0)
        {
            if(event.type == SDL_QUIT)
                return false;
            if(event.type == SDL_KEYDOWN)
                quitMenu = 1;
        }
    SDL_DestroyTexture(titleTexture);
    TTF_CloseFont(font);
    return true;
}

void Game::Init()
{
    //Board Creation
    boardSize = 50;
    CreateBoard();
    snakeLength = 3;
    board[boardSize / 2 - 1][boardSize / 2 - 1] = 1;
    snakeDir = SNAKE_DIRECTION::RIGHT;
    CreateSnake();
    srand(time(NULL));
    RenderSnake();
    CreateFood();
}

void Game::Update()
{
    SDL_Event event;
    int quit = 0;
    int canInput = 1;
    while(quit == 0)
    {
        SDL_SetRenderDrawColor(mainRenderer,0,0,0,255);
        SDL_RenderClear(mainRenderer);
        if(AddHead(snakeDir) < 0)
        {
            GameOver();
            return;
        }
        canInput = 1;
        while(SDL_PollEvent(&event) != 0)
        {
            if(event.type == SDL_QUIT)
                quit = 1;
            //INSERT INPUT RELATED FUNCTIONS HERE
            if(canInput == 1)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:
                        if(snakeDir != SNAKE_DIRECTION::UP && snakeDir != SNAKE_DIRECTION::DOWN)
                            snakeDir = SNAKE_DIRECTION::UP;
                        break;
                    case SDLK_DOWN:
                        if(snakeDir != SNAKE_DIRECTION::UP && snakeDir != SNAKE_DIRECTION::DOWN)
                            snakeDir = SNAKE_DIRECTION::DOWN;
                        break;
                    case SDLK_LEFT:
                        if(snakeDir != SNAKE_DIRECTION::LEFT && snakeDir != SNAKE_DIRECTION::RIGHT)
                            snakeDir = SNAKE_DIRECTION::LEFT;
                        break;
                    case SDLK_RIGHT:
                        if(snakeDir != SNAKE_DIRECTION::LEFT && snakeDir != SNAKE_DIRECTION::RIGHT)
                            snakeDir = SNAKE_DIRECTION::RIGHT;
                        break;
                }
                canInput = 0;
            }
            //INSERT INPUT RELATED FUNCTIONS HERE
        }
        if(board[food_y][food_x] != -1)
        {
            snakeLength++;
            CreateFood();
        }
        else
            RemoveTail();
        //system("clear");
        RenderSnake();
        //PrintBoard();
        SDL_RenderPresent(mainRenderer);
        SDL_Delay(150);
    }
}

void Game::CreateBoard()
{
    board = new int*[boardSize];
    for(int i = 0;i <= boardSize - 1;i++)
        board[i] = new int[boardSize];
    for(int i = 0;i <= boardSize - 1;i++)
        for(int j = 0;j <= boardSize - 1;j++)
            board[i][j] = 0;
}

void Game::PrintBoard()
{
    for(int i = 0;i <= boardSize - 1;i++)
    {
        for(int j = 0;j <= boardSize - 1;j++)
            std::cout << board[i][j];
        std::cout << "\n";
    }
}

void Game::CreateSnake()
{
    Snake* temp = new Snake(boardSize / 2 - 1, boardSize / 2 - 1);
    head = temp;
    for(int i = 1;i < snakeLength;i++)
    {
        temp->next = new Snake(temp->x - 1, temp->y);
        temp->next->prev = temp;
        temp = temp->next;
    }
    tail = temp;
}

int Game::AddHead(int direction)
{
    Snake* temp = NULL;
    switch(direction)
    {
        case SNAKE_DIRECTION::UP:
            if(head->y - 1 < 0)
                temp = new Snake(head->x, boardSize - 1);
            else
                temp = new Snake(head->x, head->y - 1);
            break;
        case SNAKE_DIRECTION::DOWN:
            if(head->y + 1 >= boardSize)
                temp = new Snake(head->x, 0);
            else
                temp = new Snake(head->x, head->y + 1);
            break;
        case SNAKE_DIRECTION::LEFT:
            if(head->x - 1 < 0)
                temp = new Snake(boardSize - 1, head->y);
            else
                temp = new Snake(head->x - 1, head->y);
            break;
        case SNAKE_DIRECTION::RIGHT:
            if(head->x + 1 >= boardSize)
                temp = new Snake(0, head->y);
            else
                temp = new Snake(head->x + 1, head->y);
            break;
    };
    if(board[temp->y][temp->x] != CELL_TYPE::SNAKE)
    {
        head->prev = temp;
        temp->next = head;
        head = temp;
        return 0;
    }
    else
    {
        delete temp;
        temp = NULL;
        return -1;
    }
}

void Game::RemoveTail()
{
    board[tail->y][tail->x] = CELL_TYPE::DEFAULT;
    tail = tail->prev;
    delete tail->next;
    tail->next = NULL;
}

void Game::RenderSnake()
{
    Snake* temp = head;
    SDL_SetRenderDrawColor(mainRenderer,255,255,255,255);
    SDL_Rect cell;
    while(temp != NULL)
    {
        cell = {temp->x * CELLSIZE,temp->y * CELLSIZE,CELLSIZE,CELLSIZE};
        SDL_RenderFillRect(mainRenderer,&cell);
        board[temp->y][temp->x] = CELL_TYPE::SNAKE;
        temp = temp->next;
    }
    if(board[food_y][food_x] == -1)
    {
        SDL_SetRenderDrawColor(mainRenderer,255,0,0,255);
        cell = {food_x * CELLSIZE,food_y * CELLSIZE,CELLSIZE,CELLSIZE};
        SDL_RenderFillRect(mainRenderer,&cell);
    }
}

void Game::CreateFood()
{
    do
    {
        food_x = rand() % boardSize;
        food_y = rand() % boardSize;
    }
    while(board[food_y][food_x] == CELL_TYPE::SNAKE);
    board[food_y][food_x] = CELL_TYPE::FOOD;
}

void Game::GameOver()
{
    char message[25];
    sprintf(message,"Your score is: %d",snakeLength);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Game over",message,mainWindow);
    DestroySnake();
}

void Game::DestroySnake()
{
    Snake* temp;
    while(head != NULL)
    {
        temp = head;
        head = head->next;
        delete temp;
        temp = NULL;
    }
}
