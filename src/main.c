#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

enum {
    GAME_LIST=-1,
    GAME_ASTEROIDS,
    GAME_SNAKE,
    GAME_BREAKOUT
};

static Model cabinet_model;
static Model cabinet_screen;
static RenderTexture2D cabinet_target;
static RenderTexture2D cabinet_buffer;
static int current_game = GAME_LIST;
static bool playing = false;

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
        food_on_board = 0;
        score = 0;
        grow = 0;
        elapsed = 0;
        dead = false;
        
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
    
    if (need_update)
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
    // DrawText("PLAYING SNAKE", 50, 150, 40, (Color){0, 255, 255, 255});
}

void game_breakout()
{
    if (playing) {
        if (IsKeyPressed(KEY_Q))
            current_game = GAME_LIST;
    }
    DrawText("PLAYING BREAKOUT", 50, 150, 40, (Color){0, 255, 255, 255});
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    
    InitWindow(640, 480, "1 Class Jam");
    SetTargetFPS(60);
    SetExitKey(KEY_F12);
    
    cabinet_model = LoadModel("assets/cabinet2.obj");
    cabinet_model.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/Cabinet2.png");
    cabinet_model.material.shader = LoadShader("assets/light.vs", "assets/light.fs");
    cabinet_model.material.shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(cabinet_model.material.shader, "modelMatrix");
    
    Mesh cabinet_mesh = GenMeshPlane(1.663860, 1.275940, 1, 1);
    cabinet_screen = LoadModelFromMesh(cabinet_mesh);
    cabinet_screen.material.shader = LoadShader("assets/light.vs", NULL);
    cabinet_screen.material.shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(cabinet_screen.material.shader, "modelMatrix");
    
    cabinet_target = LoadRenderTexture(512, 512);
    SetTextureFilter(cabinet_target.texture, FILTER_POINT);
    
    cabinet_buffer = LoadRenderTexture(512, 512);
    SetTextureFilter(cabinet_buffer.texture, FILTER_POINT);
    
    Camera camera = (Camera){{0.f, 3.6f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, 90.f};
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
                
                Matrix m = MatrixIdentity();
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
