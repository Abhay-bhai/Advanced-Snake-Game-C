#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

// ==================== CONSTANTS ====================
#define WIDTH 60
#define HEIGHT 20
#define INITIAL_SNAKE_LENGTH 3
#define MAX_SNAKE_LENGTH 100

// ==================== STRUCTURES ====================
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    int direction; // 1=UP, 2=RIGHT, 3=DOWN, 4=LEFT
    int score;
    int speed;
    bool isAlive;
} Snake;

typedef struct {
    Point position;
    bool isPresent;
} Food;

// ==================== GLOBAL VARIABLES ====================
Snake snake;
Food food;
int highScore = 0;

// ==================== FUNCTION PROTOTYPES ====================
void initializeGame();
void drawBoard();
void updateSnake();
void generateFood();
void checkCollisions();
void processInput();
void pauseGame();
void showStartScreen();
void showGameOver();
void clearScreen();
void saveHighScore();
void loadHighScore();

// ==================== MAIN FUNCTION ====================
int main() {
    srand(time(NULL));
    loadHighScore();
    showStartScreen();

    char choice;
    do {
        initializeGame();

        while (snake.isAlive) {
            clearScreen();
            drawBoard();
            processInput();
            updateSnake();
            checkCollisions();

            // ==================== FIXED SPEED CONTROL ====================
            int baseSpeed = 100;                  // base speed
            int decrease = snake.score / 10;      // small increment
            snake.speed = baseSpeed - decrease * 3;
            if (snake.speed < 60) snake.speed = 60; // safe minimum speed

            Sleep(snake.speed);
        }

        showGameOver();
        printf("\nPlay again? (Y/N): ");
        scanf(" %c", &choice);

    } while (choice == 'Y' || choice == 'y');

    return 0;
}

// ==================== GAME LOGIC ====================
void initializeGame() {
    snake.length = INITIAL_SNAKE_LENGTH;
    snake.direction = 2;  // start moving RIGHT
    snake.score = 0;
    snake.speed = 100;
    snake.isAlive = true;

    int sx = WIDTH / 2;
    int sy = HEIGHT / 2;

    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = sx - i;
        snake.body[i].y = sy;
    }

    food.isPresent = false;
    generateFood();
}

void drawBoard() {
    printf("╔");
    for (int i = 0; i < WIDTH; i++) printf("═");
    printf("╗\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < WIDTH; x++) {
            char ch = ' ';

            for (int i = 0; i < snake.length; i++) {
                if (snake.body[i].x == x && snake.body[i].y == y) {
                    ch = (i == 0) ? 'O' : 'o';
                    break;
                }
            }

            if (food.isPresent &&
                food.position.x == x &&
                food.position.y == y)
                ch = '@';

            printf("%c", ch);
        }
        printf("║\n");
    }

    printf("╚");
    for (int i = 0; i < WIDTH; i++) printf("═");
    printf("╝\n");

    printf("SCORE: %d   HIGH SCORE: %d   LENGTH: %d\n",
           snake.score, highScore, snake.length);
    printf("ARROWS / WASD | P=Pause | Q=Quit\n");
}

void updateSnake() {
    for (int i = snake.length - 1; i > 0; i--)
        snake.body[i] = snake.body[i - 1];

    Point head = snake.body[0];

    if (snake.direction == 1) head.y--;
    if (snake.direction == 2) head.x++;
    if (snake.direction == 3) head.y++;
    if (snake.direction == 4) head.x--;

    // WALL COLLISION
    if (head.x < 0 || head.x >= WIDTH ||
        head.y < 0 || head.y >= HEIGHT) {
        snake.isAlive = false;
        return;
    }

    snake.body[0] = head;

    if (food.isPresent &&
        head.x == food.position.x &&
        head.y == food.position.y) {

        if (snake.length < MAX_SNAKE_LENGTH) {
            snake.body[snake.length] = snake.body[snake.length - 1];
            snake.length++;
        }

        snake.score += 10;
        if (snake.score > highScore) {
            highScore = snake.score;
            saveHighScore();
        }

        food.isPresent = false;
        generateFood();
    }
}

void generateFood() {
    while (!food.isPresent) {
        food.position.x = rand() % WIDTH;
        food.position.y = rand() % HEIGHT;

        bool onSnake = false;
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.position.x &&
                snake.body[i].y == food.position.y) {
                onSnake = true;
                break;
            }
        }

        if (!onSnake)
            food.isPresent = true;
    }
}

void checkCollisions() {
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x &&
            snake.body[0].y == snake.body[i].y) {
            snake.isAlive = false;
            return;
        }
    }
}

// ==================== INPUT ====================
void processInput() {
    // UP
    if ((GetAsyncKeyState(VK_UP) & 0x8000 ||
         GetAsyncKeyState('W') & 0x8000) &&
        snake.direction != 3)
        snake.direction = 1;

    // RIGHT
    else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000 ||
              GetAsyncKeyState('D') & 0x8000) &&
             snake.direction != 4)
        snake.direction = 2;

    // DOWN
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000 ||
              GetAsyncKeyState('S') & 0x8000) &&
             snake.direction != 1)
        snake.direction = 3;

    // LEFT
    else if ((GetAsyncKeyState(VK_LEFT) & 0x8000 ||
              GetAsyncKeyState('A') & 0x8000) &&
             snake.direction != 2)
        snake.direction = 4;

    // PAUSE
    else if (GetAsyncKeyState('P') & 0x8000) {
        pauseGame();
        Sleep(200);
    }

    // QUIT
    else if (GetAsyncKeyState('Q') & 0x8000 ||
             GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        snake.isAlive = false;
}

// ==================== UI ====================
void showStartScreen() {
    clearScreen();
    printf("====== SNAKE GAME ======\n");
    printf("Press any key to start...");
    getchar();
}

void showGameOver() {
    clearScreen();
    printf("GAME OVER!\n");
    printf("Score: %d\nHigh Score: %d\n", snake.score, highScore);
}

void pauseGame() {
    clearScreen();
    printf("GAME PAUSED\nPress any key to continue...");
    getchar();
}

void clearScreen() {
    COORD c = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// ==================== FILE ====================
void saveHighScore() {
    FILE *f = fopen("snake_highscore.txt", "w");
    if (f) {
        fprintf(f, "%d", highScore);
        fclose(f);
    }
}

void loadHighScore() {
    FILE *f = fopen("snake_highscore.txt", "r");
    if (f) {
        fscanf(f, "%d", &highScore);
        fclose(f);
    }
}
