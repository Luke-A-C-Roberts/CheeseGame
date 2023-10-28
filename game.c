#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "raylib.h"

#define STAGE_WALL_WIDTH 20
#define STAGE_WALL_HEIGHT 20

#define WALL_WIDTH 20
#define WALL_X_OFFSET 50
#define WALL_Y_OFFSET 30

#define CHEESE_NUM 3

#define CHEESE_PATH "C:\\Users\\Computing\\Desktop\\cheese.png"
#define MAGE_PATH   "C:\\Users\\Computing\\Desktop\\cheesemage.png"

#define NUM_PLAYER_POSES 12

static const char
stage_layout[STAGE_WALL_WIDTH][STAGE_WALL_HEIGHT] = {
    "####################",
    "#                  #",
    "#                  #",
    "###              ###",
    "#                  #",
    "#                  #",
    "###              ###",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "###              ###",
    "#                  #",
    "#                  #",
    "###              ###",
    "#                  #",
    "#                  #",
    "####################"
};

typedef enum {
    IDLE_FORWARD,
    STEP_FORWARD1,
    STEP_FORWARD2,

    IDLE_LEFT,
    STEP_LEFT1,
    STEP_LEFT2,

    IDLE_RIGHT,
    STEP_RIGHT1,
    STEP_RIGHT2,

    IDLE_BACK,
    STEP_BACK1,
    STEP_BACK2,
} PlayerPose;

typedef struct {
    int xpos;
    int ypos;
    int width;
    int height;
    float direction;
    float velocity;
    int hp;
    PlayerPose player_pose;
} Player;

typedef struct {
    int xpos;
    int ypos;
} Wall;

typedef struct {
    int xpos;
    int ypos;
    bool eaten;
} Cheese;

int init_walls(Wall** walls) {
    unsigned int walls_count = 1;

    for (int y = 0; y < STAGE_WALL_HEIGHT; ++y) {
        for (int x = 0; x < STAGE_WALL_WIDTH; ++x) {
            if (stage_layout[x][y] == '#')
                walls_count++;
        }
    }

    Wall* new_walls = malloc(walls_count * sizeof(Wall));
    unsigned int array_pos = 0;

    for (int y = 0; y < STAGE_WALL_HEIGHT; ++y) {
        for (int x = 0; x < STAGE_WALL_WIDTH; ++x) {
            if (stage_layout[x][y] == '#') {
                new_walls[array_pos] = (Wall) {
                    .xpos = x * WALL_WIDTH + WALL_X_OFFSET,
                    .ypos = y * WALL_WIDTH + WALL_Y_OFFSET
                };
                array_pos++;
            }
        }
    }
    *walls = new_walls;
    return walls_count;
}

void show_fps(void) {
    char buffer[20];
    int fps = GetFPS();
    sprintf(buffer, "FPS: %d", fps);
    DrawText(buffer, 50, 50, 10, LIGHTGRAY);
}

void show_walls(const Wall* walls, const int walls_size) {

    for (int i = 0; i < walls_size; ++i) {
        Wall wall = walls[i];
        DrawRectangle(wall.xpos, wall.ypos, WALL_WIDTH, WALL_WIDTH, BLACK);
    }
}

bool wall_collision(const Wall wall, const int x, const int y) {
    return(
           x > wall.xpos
        && x < wall.xpos + WALL_WIDTH
        && y > wall.ypos
        && y < wall.ypos + WALL_WIDTH
    )? true : false;
}

void spawn_cheeses(Cheese* cheeses, const Wall* walls, const int walls_size) {
    for (int c = 0; c < CHEESE_NUM; ++c) {
        
        int rand_x;
        int rand_y;

        bool rand_invalid = true;
        while (rand_invalid) {
            rand_x = (int)floor(((float)rand() / RAND_MAX) * STAGE_WALL_WIDTH  * WALL_WIDTH + WALL_X_OFFSET);
            rand_y = (int)floor(((float)rand() / RAND_MAX) * STAGE_WALL_HEIGHT * WALL_WIDTH + WALL_Y_OFFSET);
            
            rand_invalid = false;
            for (int w = 0; w < walls_size; ++w) {
                if (wall_collision(walls[w], rand_x, rand_y)) {
                    rand_invalid = true;
                    break;
                }
            }
        }

        Cheese new_cheese = (Cheese) {
            .xpos = rand_x,
            .ypos = rand_y,
            .eaten = false
        };

        cheeses[c] = new_cheese;
    }
}

void show_cheeses(const Cheese* cheeses, const Texture2D* cheese_texture) {
    
    for (int c = 0; c < CHEESE_NUM; ++c) {
        Cheese cheese = cheeses[c];
        DrawTexture(*cheese_texture, cheese.xpos, cheese.ypos, YELLOW);
    }
}

void create_player_textures(Texture2D* textures, const Image* image) {
    for (int p = 0; p < NUM_PLAYER_POSES; ++p) {
        bool flip = false;
        if (p >= 4 || p < 7) {
            flip = true;
            p -= 3;
        }
        if (p <= 7) {
            p -= 3;
        }

        Image im_slider = ImageFromImage(
            *image,
            (Rectangle) {
                .x = 16 * p,
                .y = 0,
                .width = 16,
                .height = 16
            }
        );
        
        if (flip) ImageFlipHorizontal(&im_slider);

        Texture2D new_texture = LoadTextureFromImage(im_slider);
        textures[p] = new_texture;
    }


}

void show_player(const Player* player) {
    
}

int main(void) {
    srand(time(NULL));

    InitWindow(800, 450, "Game Window");
    SetTargetFPS(60);

    MaximizeWindow();

    Wall* walls = NULL;
    int walls_size = init_walls(&walls);

    if (walls_size == -1) {
        printf("NULL realloc while trying to create walls!\n");
        return EXIT_FAILURE;
    }

    Cheese cheeses[CHEESE_NUM];
    spawn_cheeses(cheeses, walls, walls_size);
    Texture2D cheese_texture = LoadTexture(CHEESE_PATH);

    Player player = (Player) {
        .xpos        = 100,
        .ypos        = 100,
        .width       = 20,
        .height      = 20,
        .direction   = 0,
        .velocity    = 0,
        .hp          = 100,
        .player_pose = IDLE_FORWARD
    };
    Image player_image = LoadImage(MAGE_PATH);
    Texture player_textures[NUM_PLAYER_POSES];
    create_player_textures(player_textures, &player_image);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        show_fps();
        show_walls(walls, walls_size);
        show_cheeses(cheeses, &cheese_texture);

        EndDrawing();
    }

    CloseWindow();

    free(walls);

    return EXIT_SUCCESS;
}
