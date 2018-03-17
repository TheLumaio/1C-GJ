#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>



#define GAME_LIST      -1
#define GAME_ASTEROIDS  0
#define GAME_SNAKE      1
#define GAME_BREAKOUT   2

#define BO_X    0
#define BO_Y    1
#define BO_FLAG 2

static Model cabinet_model;
static Model cabinet_joystick;
static Model cabinet_screen;
static Camera camera;

static RenderTexture2D cabinet_target;
static RenderTexture2D cabinet_buffer;
static int current_game = GAME_LIST;
static bool playing = false;

bool check_collide(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    bool c = false;
    float dx = fabs((x1+w1/2.f) - (x2+w2/2.f));
    float dy = fabs((y1+h1/2.f) - (y2+h2/2.f));
    if ((dx <= (w1/2.f + w2/2.f)) && ((dy <= (h1/2.f + h2/2.f)))) c = true;
    return c;
}

void game_list()
{
    static int selection = 0;
    /* Instead of wrapping updates in a separate function I just check if you're playing in each game update */
    if (playing) {
        if (IsKeyPressed(KEY_DOWN))
            selection = (selection+1)%3;
        if (IsKeyPressed(KEY_UP)) {
            selection--;
            if (selection < 0) selection = 2;
        }
        if (IsKeyPressed(KEY_ENTER))
            current_game = selection;
    }
    
    DrawRectangle(100, 150+selection*40, 300, 40, GRAY);
    
    DrawText("ASTEROIDS", 100, 150, 40, GREEN);
    DrawText("SNAKE", 100, 150+40, 40, GREEN);
    DrawText("BREAKOUT", 100, 150+40+40, 40, GREEN);
}

void game_asterioids()
{
    if (playing) {
        if (IsKeyPressed(KEY_Q))
            current_game = GAME_LIST;
    }
    DrawText("PLAYING ASTEROIDS", 50, 150, 40, (Color){0, 255, 255, 255});
}

void game_snake()
{
    static bool snake_initialized = false;
    static int board[16][16];
    static int food_on_board = 0;
    static const int max_food = 1;
    static int head_x = 10;
    static int head_y = 8;
    static int tail_x = 7;
    static int tail_y = 8;
    static float elapsed = 0.f;
    static char direction = 'e';
    static const int grow_amount = 1;
    static int grow = 0;
    static bool dead = false;
    static int score = 0;
    static bool need_update = false;
    
    if (!snake_initialized) {
        head_x = 10;
        head_y = 8;
        tail_x = 7;
        tail_y = 8;
        direction = 'e';
        score = 0;
        grow = 0;
        elapsed = 0;
        dead = false;
        need_update = false;
        
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
                board[i][j] = 0;
        for (int i = tail_x; i <= head_x; i++) {
            board[i][head_y] = 'e';
        }
        board[rand()%16][rand()%16] = 1;
        snake_initialized = true;
    }
    
    if (playing) {
        elapsed += GetFrameTime();
        
        if (IsKeyPressed(KEY_Q))
            current_game = GAME_LIST;
        
        if (!dead) {
            if (IsKeyPressed(KEY_UP) && direction != 's' && direction != 'n') {
                elapsed = 0;
                board[head_x][head_y] = 'n';
                head_y--;
                direction = 'n';
                need_update = true;
            }
            
            if (IsKeyPressed(KEY_DOWN) && direction != 's' && direction != 'n') {
                elapsed = 0;
                board[head_x][head_y] = 's';
                head_y++;
                direction = 's';
                need_update = true;
            }
            
            if (IsKeyPressed(KEY_LEFT) && direction != 'e' && direction != 'w') {
                elapsed = 0;
                board[head_x][head_y] = 'w';
                head_x--;
                direction = 'w';
                need_update = true;
            }
            
            if (IsKeyPressed(KEY_RIGHT) && direction != 'e' && direction != 'w') {
                elapsed = 0;
                board[head_x][head_y] = 'e';
                head_x++;
                direction = 'e';
                need_update = true;
            }
            if (head_x<0||head_x>15||head_y<0||head_y>15 || (need_update && board[head_x][head_y] > 1)) {
                printf("SNEK DED\n");
                dead = true;
            }
        } else {
            if (IsKeyPressed(KEY_R))
                snake_initialized = false;
        }
    }
    
    if (elapsed > 0.1) {
        elapsed = 0;
        switch (direction) {
            case 'n':
                if (head_y-1<0) {
                    dead = true;
                    break;
                }
                board[head_x][head_y] = 'n';
                head_y--;
                if (board[head_x][head_y] > 1) {
                    dead = true;
                    break;
                }
                need_update = true;
                break;
            case 'e':
                if (head_x+1>15) {
                    dead = true;
                    break;
                }
                board[head_x][head_y] = 'e';
                head_x++;
                if (board[head_x][head_y] > 1) {
                    dead = true;
                    break;
                }
                need_update = true;
                break;
            case 's':
                if (head_y+1>15) {
                    dead = true;
                    break;
                }
                board[head_x][head_y] = 's';
                head_y++;
                if (board[head_x][head_y] > 1) {
                    dead = true;
                    break;
                }
                need_update = true;
                break;
            case 'w':
                if (head_x-1<0) {
                    dead = true;
                    break;
                }
                board[head_x][head_y] = 'w';
                head_x--;
                if (board[head_x][head_y] > 1) {
                    dead = true;
                    break;
                }
                need_update = true;
                break;
            default:
                break;
        }
        
    }
    
    if (need_update && !dead)
    {
        if (board[head_x][head_y] == 1) {
            grow = grow_amount;
            score++;
            int a=rand()%16;
            int b=rand()%16;
            
            while (board[a][b] != 0) {
                a=rand()%16;
                b=rand()%16;
            }
            board[a][b] = 1;
        }
        board[head_x][head_y] = 2;
        
        /* pop tail */
        if (grow > 0)
            grow--;
        else {
            int a=tail_x,b=tail_y;
            switch (board[tail_x][tail_y]) {
                case 'n':
                    tail_y--;
                    break;
                case 'e':
                    tail_x++;
                    break;
                case 's':
                    tail_y++;
                    break;
                case 'w':
                    tail_x--;
                    break;
                default:
                    break;
            }
            board[a][b] = 0;
        }
        need_update = false;
    }
    
    if (!dead) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                Color c = BLUE;
                if (board[i][j] == 0) c = WHITE;
                if (board[i][j] == 1) c = GREEN;
                DrawRectangle(100+i*20, 100+j*20, 15, 15, c);
            }
        }
        DrawText(FormatText("Score: %d", score), 150, 475, 40, GREEN);
    } else {
        DrawText("You died", 100, 150, 40, RED);
        DrawText(FormatText("Score: %d", score), 100, 200, 40, RED);
        DrawText("R to restart", 100, 250, 40, GREEN);
        DrawText("Q to exit", 100, 300, 40, GREEN);
    }
}

void game_breakout()
{
    static bool breakout_initalized = false;
    static const int width = 6;
    static const int height = 6;
    static int blocks[width*height][3];
    static float ball_x;
    static float ball_y;
    static float bvel_x;
    static float bvel_y;
    static int paddle_width;
    static float paddle_x;
    static bool stuck;
    static bool dead = false;
    static int difficulty = 1;
    static int score;
    static int blocks_left;
    
    if (!breakout_initalized) {
        ball_x = 0;
        ball_y = 0;
        paddle_width = 100;
        paddle_x = 256-paddle_width/2;
        stuck = true;
        bvel_x = 0;
        bvel_y = 100;
        dead = false;
        blocks_left = width*height;
        
        for (int i = 0; i < width*height; i++) {
            int a = i%width;
            int b = i/width;
            
            blocks[i][BO_X]    = 75+a*60; // X
            blocks[i][BO_Y]    = 75+b*20; // Y
            blocks[i][BO_FLAG] = 1+rand()%difficulty; // FLAG (number of hits required to break)
        }
        
        breakout_initalized = true;
    }
    
    if (playing) {
        if (IsKeyPressed(KEY_Q))
            current_game = GAME_LIST;
        
        if (!dead) {
            if (IsKeyDown(KEY_LEFT))
                paddle_x -= 300*GetFrameTime();
            if (IsKeyDown(KEY_RIGHT))
                paddle_x += 300*GetFrameTime();
            if (IsKeyPressed(KEY_SPACE) && stuck)
                stuck = false;
        } else {
            if (IsKeyPressed(KEY_R)) {
                breakout_initalized = false;
                difficulty = 1;
                score = 0;
            }
        }
        
    }
    
    if (dead) goto DEAD; /* because im lazy */
    
    for (int i = 0; i < width*height; i++)
    {
        if (blocks[i][BO_FLAG] == 0) continue;
        int f = blocks[i][BO_FLAG];
        Color c = WHITE;
        if (f == 2) c = GREEN;
        if (f == 3) c = BLUE;
        if (f == 4) c = YELLOW;
        DrawRectangle(blocks[i][BO_X], blocks[i][BO_Y], 55, 15, c);
    }
    
    if (stuck) {
        ball_x = paddle_x+paddle_width/2-10;
        ball_y = 450-20;
    } else {
        if (bvel_x > 300) bvel_x = 300;
        if (bvel_y > 300) bvel_y = 300;
        if (bvel_x < -300) bvel_x = -300;
        if (bvel_y < -300) bvel_y = -300;
        
        ball_x += bvel_x*GetFrameTime();
        ball_y += bvel_y*GetFrameTime();
        
        /* Edge collision detection */
        if (ball_x < 0) {
            bvel_x = -bvel_x;
            ball_x = 0;
        }
        if (ball_x+20 > 512) {
            bvel_x = -bvel_x;
            ball_x = 512-20;
        }
        if (ball_y < 0) {
            bvel_y = -bvel_y;
            ball_y = 0;
        }
        if (ball_y+20 > 512) {
            dead = true;
            goto DEAD;
        }
        
        /* Paddle collision detection */
        bool c_paddle = false;
        c_paddle = check_collide(ball_x, ball_y, 20, 20, paddle_x, 450, paddle_width, 15);
        if (c_paddle) {
            if (bvel_x < 0) bvel_x -= 50;
            else bvel_x += 50;
            if (bvel_y < 0) bvel_y -= 50;
            else bvel_y += 50;
            
            
            bvel_y = -bvel_y;
            if (bvel_x == 0) bvel_x = 100;
            if (ball_x+10 < paddle_x+paddle_width/2)
                bvel_x = -fabs(bvel_x);
            else
                bvel_x = fabs(bvel_x);
            
            if (ball_y < 450)
                ball_y = 450-20;
            else
                ball_y = 450;
        }
        
        /* Block collision detection */
        float dx = ball_x+10;
        float dy = ball_y+10;
        
        for (int i = 0; i < width*height; i++)
        {
            if (blocks[i][BO_FLAG] == 0) continue;
            int x = blocks[i][BO_X];
            int y = blocks[i][BO_Y];
            
            if (check_collide(x, y, 55, 15, ball_x, ball_y, 20, 20)) {
                blocks[i][BO_FLAG]--;
                if (blocks[i][BO_FLAG] == 0) {
                    blocks_left--;
                    score++;
                }
                if (dy < y || dy > y+15) {
                    bvel_y = -bvel_y;
                    break;
                }
                if (dx < x || dx > x+55) {
                    bvel_x = -bvel_x;
                    break;
                }
                break;
            }
            
        }
        
        if (blocks_left == 0) {
            stuck = true;
            bvel_x = 0;
            bvel_y = 100;
            if (difficulty < 4) difficulty++;
            breakout_initalized = false;
        }
        
    }
    
    DrawText(FormatText("Score: %d", score), 150, 475, 40, GREEN);
    DrawRectangle(ball_x, ball_y, 20, 20, (Color){0, 255, 255, 255});
    DrawRectangle(paddle_x, 450, paddle_width, 15, WHITE);
    
    DEAD:
    if (dead) {
        DrawText("You died", 100, 150, 40, RED);
        DrawText(FormatText("Score: %d", score), 100, 200, 40, RED);
        DrawText("R to restart", 100, 250, 40, GREEN);
        DrawText("Q to exit", 100, 300, 40, GREEN);
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    
    
    InitWindow(640, 480, "1 Class Jam");
    SetTargetFPS(60);
    SetExitKey(KEY_F12);
    
    SetMousePosition((Vector2){0, 0});
    
    Shader light = LoadShader("assets/light.vs", "assets/light.fs");
    light.locs[LOC_MATRIX_MODEL] = GetShaderLocation(light, "modelMatrix");
    
    cabinet_model = LoadModel("assets/cabinet2.obj");
    cabinet_model.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/Cabinet2.png");
    cabinet_model.material.shader = light;
    
    cabinet_joystick = LoadModel("assets/joystick.obj");
    cabinet_joystick.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/Joystick.png");
    cabinet_joystick.material.shader = light;
    
    Mesh cabinet_mesh = GenMeshPlane(1.663860, 1.275940, 1, 1);
    cabinet_screen = LoadModelFromMesh(cabinet_mesh);
    cabinet_screen.material.shader = LoadShader("assets/light.vs", NULL);
    
    cabinet_target = LoadRenderTexture(512, 512);
    SetTextureFilter(cabinet_target.texture, FILTER_POINT);
    
    cabinet_buffer = LoadRenderTexture(512, 512);
    SetTextureFilter(cabinet_buffer.texture, FILTER_POINT);
    
    camera = (Camera){{3.f, 3.6f, 3.f}, {0.f,3.f, 0.f}, {0.f, 1.f, 0.f}, 90.f};
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    while (!WindowShouldClose())
    {
        if (!playing) {
            if (IsKeyPressed(KEY_E)) 
                playing = true;
            else /* Because of a minor graphical bug */
                UpdateCamera(&camera);
        } else {
            if (IsKeyPressed(KEY_ESCAPE))
                playing = false;
        }
        
        BeginDrawing();
        
            ClearBackground(BLACK);
            
            BeginTextureMode(cabinet_target);
                ClearBackground((Color){10, 10, 10, 255});
                switch(current_game)
                {
                    case GAME_LIST:
                        game_list();
                        break;
                    case GAME_ASTEROIDS:
                        game_asterioids();
                        break;
                    case GAME_SNAKE:
                        game_snake();
                        break;
                    case GAME_BREAKOUT:
                        game_breakout();
                        break;
                    default:
                        break;
                }
            EndTextureMode();
            
            /* Cheap hack because textures get inverted on planes for some reason */
            BeginTextureMode(cabinet_buffer);
                DrawTextureRec(cabinet_target.texture, (Rectangle){0, 0, -512, -512}, (Vector2){0, 0}, WHITE);
            EndTextureMode();
            
            Begin3dMode(camera);
                DrawModel(cabinet_model, (Vector3){0, 0, 0}, 1.f, WHITE);
                
                float rx = 0;
                float rz = 0;
                if (IsKeyDown(KEY_UP)) rx = 5;
                else if (IsKeyDown(KEY_DOWN)) rx = -5;
                if (IsKeyDown(KEY_LEFT)) rz = -5;
                else if (IsKeyDown(KEY_RIGHT)) rz = 5;
                
                Matrix m = MatrixIdentity();
                m = MatrixMultiply(m, MatrixRotateX(rx*DEG2RAD));
                m = MatrixMultiply(m, MatrixRotateZ(rz*DEG2RAD));
                cabinet_joystick.transform = m;
                DrawModel(cabinet_joystick, (Vector3){-0.002223, 2.79611, 0.690871}, 1.f, WHITE);
                
                m = MatrixIdentity();
                m = MatrixMultiply(m, MatrixRotateX(19.6*DEG2RAD));
                m = MatrixMultiply(m, MatrixRotateY(180*DEG2RAD));
                cabinet_screen.transform = m;
                cabinet_screen.material.maps[MAP_DIFFUSE].texture = cabinet_buffer.texture;
                DrawModel(cabinet_screen, (Vector3){-0.003, 2.93800, -0.251312}, 1.f, WHITE);
            End3dMode();
            
        EndDrawing();
    }
    
    UnloadModel(cabinet_model);
    UnloadModel(cabinet_screen);
    UnloadRenderTexture(cabinet_target);
    UnloadRenderTexture(cabinet_buffer);
    
    CloseWindow();
    
    return 0;
}
