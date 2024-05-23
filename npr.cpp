#include <raylib.h>
#include <iostream>

using namespace std;

class Paddle {

public:
    float x, y;
    float width, height;
    int speed_y, speed_x;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x,y, width, height}, 0.8, 0, WHITE);
    }

    void Update(int up_key, int down_key, int left_key, int right_key, int boundary_left, int boundary_right)
    {
        if (IsKeyDown(up_key) && y > 0)
        {
            y -= speed_y;
        }

        if (IsKeyDown(down_key) && (y + height) <= GetScreenHeight())
        {
            y += speed_y;
        }

        if (IsKeyDown(left_key) && x > boundary_left)
        {
            x -= speed_x;
        }

        if (IsKeyDown(right_key) && x < boundary_right)
        {
            x += speed_x;
        }
    }
};

class Ball : public Paddle {

public:
    float x, y;
    float speed_x, speed_y;
    int radius;
    int score1 = 0;
    int score2 = 0;

    void Draw()
    {
        DrawCircle(x, y, radius, PINK);
    }

    bool Winner() {
        if(score2 == 10) {
            DrawText("Your Win!", GetScreenWidth()/2 + 50, GetScreenHeight()/2, 60, PINK);
            speed_x = 0;
            speed_y = 0;
            return true;
        }
        else if( score1 == 10) {
            DrawText("You Win!",  50, GetScreenHeight()/2, 60, PINK);
            speed_x = 0;
            speed_y = 0;
            return true;
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1,1};
        speed_x *= speed_choices[GetRandomValue(0,1)];
        speed_y *= speed_choices[GetRandomValue(0,1)];
    }

    void Update(Paddle p1, Paddle p2, Sound reb, Sound faill, Sound pad)
    {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
            PlaySound(reb);
        }

        if (x <= 0)
        {
            ResetBall();
            score2++;
            PlaySound(faill);
        }

        if (x >= GetScreenWidth())
        {   
            ResetBall();
            score1++;
            PlaySound(faill);
        }

        bool overlapLeft = (x + radius >= p1.x && x - radius <= p1.x + p1.width) ? true : false;
        bool overlapRight = (x + radius >= p2.x && x + radius <= p2.x + p2.width) ? true : false;
        bool collisionLeft = CheckCollisionCircleRec({x, y}, radius, {p1.x, p1.y, p1.width, p1.height});
        bool collisionRight = CheckCollisionCircleRec({x, y}, radius, {p2.x, p2.y, p2.width, p2.height});

        if (overlapRight && y + radius >= p2.y && y - radius <= p2.y + p2.height)
        {
            while(true) {

                if(IsKeyDown(KEY_LEFT)) {
                    x = p2.x - radius*2;
                    speed_x *= -1.15;
                    speed_y *= 1.15;
                    PlaySound(pad);
                    break;
                }
                else if(IsKeyDown(KEY_RIGHT)){ 
                    x = p2.x - radius*2;
                    speed_x *= -0.75;
                    speed_y *= 0.75;
                    PlaySound(pad);
                    break;;
                }
                else{
                    x = p2.x - radius*2;
                    speed_x *= -0.9;
                    PlaySound(pad);
                    break;;
                }
            }
        }

        else if (overlapRight && collisionRight && y >= p2.y + p2.height)
        {

            y = p2.y + p2.height + radius;
            PlaySound(pad);
            speed_x *= -1;
        }

        if(overlapLeft && y + radius >= p1.y && y - radius <= p1.y + p1.height) {
             if(IsKeyDown(KEY_D)) {
                x = p1.x + p1.width*2 -5;
                speed_x *= -1.15;
                speed_y *= 1.15;
                PlaySound(pad);
            }
            else if(IsKeyDown(KEY_A)){ 
                x = p1.x + p2.width*2 -5;
                speed_x *= -0.75;
                speed_y *= 0.75;
                PlaySound(pad);
            }
            else{
                x = p1.x + p1.width*2 -5;
                speed_x *= -1;
                PlaySound(pad);
        }
        }

        else if( overlapLeft && collisionLeft && y - radius >= p2.y + p2.height) {
            y = p1.y + p1.height + radius;
            speed_y *= -1;
            PlaySound(pad);
        }
    }
};

Paddle player1;
Paddle player2;
Ball ball;

int main()
{
    InitAudioDevice();

    Sound rebound = LoadSound("resources/hit.wav");
    Sound paddle_hit = LoadSound("resources/paddle_sound.wav");
    Sound fail = LoadSound("resources/fail_sound.wav");
    Music music = LoadMusicStream("resources/melody.mp3");

    Color LighterPurple = Color{145, 96, 235, 255};
    Color Pink = Color{218, 22, 240, 255};
    Color pinkier = Color{197, 134, 252, 150};
    Color DarkerPurple = Color{122, 38, 224, 255};
    Color Purple = Color{127, 49, 222, 255};

    const int screen_width = 1900;
    const int screen_height = 1000;
    InitWindow(screen_width, screen_height, "PONG");
    SetTargetFPS(60);

    ball.radius = 15;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 8;
    ball.speed_y = 8;

    player1.width = 25;
    player1.height = 150;
    player1.x = 10;
    player1.y = screen_height / 2 - player2.height / 2;
    player1.speed_y = 8;
    player1.speed_x = 8;

    player2.width = 25;
    player2.height = 150;
    player2.x = screen_width - player1.width - 10;
    player1.y = screen_height / 2 - player1.height / 2;
    player2.speed_y = 8;
    player2.speed_x = 8;

    PlayMusicStream(music);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        BeginDrawing();

        if(ball.Winner()) {
            WindowShouldClose();
        }
        EndDrawing();
        
        string message1 = to_string(ball.score1);
        string message2 = to_string(ball.score2);

        DrawRectangle(0, 0, screen_width/2, screen_height, Purple);
        DrawCircle(screen_width/2, screen_height/2, 200, pinkier);
        DrawText(message1.c_str(), (screen_width / 2) / 2, screen_height / 2 - 100, 200, LighterPurple);
        DrawText(message2.c_str(), screen_width / 2 + (screen_width / 2) / 2, screen_height / 2 - 100, 200, LighterPurple);
        ball.Update(player1, player2, rebound, fail, paddle_hit);
        player1.Update(KEY_W, KEY_S, KEY_A, KEY_D, 10,screen_width/2 - (player2.width + 10));
        player2.Update(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, screen_width/2 + 10, screen_width - (player1.width + 10));
        ClearBackground(DarkerPurple);

        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        player1.Draw();
        player2.Draw();
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
