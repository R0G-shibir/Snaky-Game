#include <bits/stdc++.h>
#include "include/raylib.h"
#include "include/raymath.h"
using namespace std;

Color green = {173,204,96,255};
Color darkGreen = {43,51,24,255};

int cellSize = 30;
int cellNumber = 25;
int offset =75;

bool ElementInDeque(Vector2 element,deque<Vector2> deq){
    for(unsigned int i=0;i<deq.size();i++){
        if(Vector2Equals(element,deq[i])){
            return true;
        }
    }
    return false;
}

double lastUpdateTime = 0;
bool time(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake {
    public :
        deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        Vector2 direction = {1,0};
        bool AddBody = false;

        void Draw(){
            for(unsigned int i=0;i<body.size();i++){
                float x = body[i].x;
                float y = body[i].y;
                Rectangle rec = {offset+x*cellSize,offset+y*cellSize,(float)cellSize,(float)cellSize};
                DrawRectangleRounded(rec,0.5,6,darkGreen);
            }
        }
        void Move(){
            body.push_front(Vector2Add(body[0],direction));
            if(AddBody){
                AddBody = false;
            }
            else{
                body.pop_back();
            }
        }
        void Reset(){
            body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
            direction = {1,0};
        }
};

class Food {
    public:
        Vector2 position;
        Texture2D texture;
        Food(deque<Vector2> snakeBody){
            Image image = LoadImage("grafics/food.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);    
            position = randomPosition(snakeBody);
        }
        ~Food(){
            UnloadTexture(texture);
        }
        void Draw(){
            DrawTexture(texture,offset+position.x*cellSize,offset+position.y*cellSize,WHITE);
        }
        Vector2 randomCell(){
            float x = GetRandomValue(0,cellNumber-1);
            float y = GetRandomValue(0,cellNumber-1);
            return Vector2{x,y};
        }
        Vector2 randomPosition(deque<Vector2> snakeBody){    
            Vector2 position = randomCell();
            while(ElementInDeque(position,snakeBody)){
                position = randomCell();
            }
            return position;
        }   
};

class Game{
    public :
        Snake snake = Snake();
        Food food= Food(snake.body);
        bool running = true;
        int score = 0;
        Sound eatSound;
        Sound gameOverSound;
        Game(){
            InitAudioDevice();
            eatSound = LoadSound("audio/eat.mp3");
            gameOverSound = LoadSound("audio/gameOver.mp3");
        }
        ~Game(){
            UnloadSound(eatSound);
            UnloadSound(gameOverSound);
            CloseAudioDevice();
        }
        void Draw(){
            food.Draw();
            snake.Draw();
        }
        void Move(){
            if(running){
                snake.Move();
                CheckCollisionWithFood();
                CheckCollisionWithWall();
                CheckCollisionWithItself();
            }
        }
        void CheckCollisionWithFood(){
            if(Vector2Equals(snake.body[0],food.position)){
                food.position = food.randomPosition(snake.body);
                snake.AddBody = true;
                score++;
                PlaySound(eatSound);
            }
        }
        void CheckCollisionWithWall(){
            if(snake.body[0].x == cellNumber || snake.body[0].x == -1 || snake.body[0].y == cellNumber || snake.body[0].y == -1){
                GameOver();
            }
        }
        void GameOver(){
            snake.Reset();
            food.position = food.randomPosition(snake.body);
            running = false;
            score = 0;
            PlaySound(gameOverSound);
        }
        void CheckCollisionWithItself(){
            deque<Vector2> headLessBody = snake.body;
            headLessBody.pop_front();
            if(ElementInDeque(snake.body[0],headLessBody)){
                GameOver();
            }
        }
};

int main(){
    cout<<"the game is starting .... !"<<endl;
    InitWindow(2*offset+cellSize*cellNumber,2*offset+cellSize*cellNumber,"SNAKY");
    SetTargetFPS(60);
    Game game;
    while(!WindowShouldClose()){
        BeginDrawing();
        if(time(0.2)){
            game.Move();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0,-1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0,1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.running = true;
        }
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellSize*cellNumber+10,(float)cellSize*cellNumber+10},5,darkGreen);
        DrawText("SNAKY",offset-5,20,40,darkGreen);
        DrawText(TextFormat("%i",game.score), offset-5,offset +cellSize*cellNumber+10,40,darkGreen);
        game.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}