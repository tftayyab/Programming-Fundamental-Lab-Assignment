#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <GL/glut.h>

#define cols 25
#define rows 25
#define foods 1
#define logo 8

#define __ES_INCLUDE_IMAGE
#include "snake_tileset.c"
unsigned int textureId;

char board[cols * rows];

int isGameOver = 0;
int gameOverTimeout = 0;
int deltaX = 0, deltaY = 0;

#define SNAKE_MAX_LEN 256
struct SanpPart {  // Anoosh: Defines a part of the snake with x and y coordinates
    int x, y;
};
struct Sanp {  // Anoosh: Represents the snake with a length and an array of parts
    int length;
    struct SanpPart part[SNAKE_MAX_LEN];
};
struct Sanp sanp;

struct Food {  // Anoosh: Represents food with x, y coordinates and a consumed status
    int x, y;
    int consumed;
};
struct Food food[foods];

void fill_board() {  // Tayyab: Fills the game board with borders and empty spaces
    int x, y;

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1) {
                board[y * cols + x] = '#';  // Border
            }
            else {
                board[y * cols + x] = isGameOver ? 'X' : ' ';  // Empty space
            }
        }
    }
}

void draw_snake() {  // Tayyab: Draws the snake on the board based on its position
    int i;

    // Draw the snake's body
    for (i = sanp.length - 1; i > 0; i--) {
        if (sanp.part[i].x > -1 || sanp.part[i].y > -1) {
            board[sanp.part[i].y * cols + sanp.part[i].x] = '*';  // Snake body
        }
    }

    // Draw the snake's head
    board[sanp.part[0].y * cols + sanp.part[0].x] = '@';

    // Add the '~' mark for movement, if applicable
    if (deltaX || deltaY) {  // Snake must be moving
        if ((sanp.part[0].x > 0 && sanp.part[0].x < (cols - 1)) &&
            (sanp.part[0].y > 0 && sanp.part[0].y < (rows - 1))) {
            board[(sanp.part[0].y + deltaY) * cols + (sanp.part[0].x + deltaX)] = '~';
        }
    }
}


void snake_move(int deltaX, int deltaY) {  // Essa: Moves the snake by updating its coordinates
    int i;

    for (i = sanp.length - 1; i > 0; i--) {
        sanp.part[i] = sanp.part[i - 1];  // Shift the snake's body
    }

    sanp.part[0].x += deltaX;  // Move head in X direction
    sanp.part[0].y += deltaY;  // Move head in Y direction
}

void draw_food() {  // Hanzala: Draws food on the board if it has not been consumed
    if (!food[0].consumed) {
        board[food[0].y * cols + food[0].x] = '+';  // Food
    }
}

void setup_food() {  // Hanzala: Sets up the food positions randomly on the board, ensuring it's not on the snake
    int foodX, foodY;
    int validPosition = 0;

    // Continue generating random positions until a valid one is found
    while (!validPosition) {
        foodX = 1 + rand() % (cols - 2);  // Random X position within bounds
        foodY = 1 + rand() % (rows - 2);  // Random Y position within bounds

        validPosition = 1;  // Assume valid position, will set to 0 if any conflict with snake

        // Check if the food position overlaps with any part of the snake's body
        for (int i = 0; i < sanp.length; i++) {
            if (sanp.part[i].x == foodX && sanp.part[i].y == foodY) {
                validPosition = 0;  // Food position is invalid, need to try again
                break;
            }
        }
    }

    // If a valid position is found, set the food's position and mark it as not consumed
    food[0].x = foodX;
    food[0].y = foodY;
    food[0].consumed = 0;
}


void sanp_setter() { //// Tayyab: Initializes the snake with a random position and length of 1
    int i = 0;
    int snakePlaced = 0;
    sanp.length = 1;

    // Attempt to place the snake's head in a valid position
    while (!snakePlaced) {
        snakePlaced = 1;
        sanp.part[0].x = 1 + rand() % (cols - 2);
        sanp.part[0].y = 1 + rand() % (rows - 2);

        // Ensure the snake's head does not overlap with any food
        for (i = 0; i < foods; i++) {
            if (food[i].x == sanp.part[0].x && food[i].y == sanp.part[0].y) {
                snakePlaced = 0;  // Invalid placement, try again
                break;
            }
        }
    }

    // Initialize remaining parts of the snake to an off-screen position
    for (i = 1; i < SNAKE_MAX_LEN; i++) {
        sanp.part[i].x = -1;
        sanp.part[i].y = -1;
    }
}



void game_rules() {  // Hanzala: Checks the game rules like collision and food consumption
    // Check if the snake eats the food
    if (food[0].x == sanp.part[0].x && food[0].y == sanp.part[0].y) {
        food[0].consumed = 1;  // Mark food as consumed
        sanp.length++;  // Increase snake length
        setup_food();  // Regenerate food at a new valid position
    }

    // Check for wall collisions
    if (sanp.part[0].x == 0 || sanp.part[0].x == cols - 1 ||
        sanp.part[0].y == 0 || sanp.part[0].y == rows - 1) {
        isGameOver = 1;  // Game over if snake hits a wall
    }

    // Check if the snake collides with itself
    for (int i = 1; i < sanp.length; i++) {
        if (sanp.part[0].x == sanp.part[i].x && sanp.part[0].y == sanp.part[i].y) {
            isGameOver = 1;  // Game over if snake collides with itself
        }
    }
}


void displayFunc() {

    int y, x;
    static int lastDeltaX = 0, lastDeltaY = 0;

    static struct {
        int x1, y1, x2, y2;
    } tiles[] = {
        { 24, 16,  31, 23 }, // '#' Wall
        {  8, 24,  15, 33 }, // '@' Up
        {  0, 16,   7, 23 }, // '@' Down
        {  8,  8,  15, 15 }, // '@' Left
        {  0,  0,   7,  7 }, // '@' Right
        {  8, 16,  15, 23 }, // '~' Up
        {  0, 24,   7, 33 }, // '~' Down
        {  0,  8,   7, 15 }, // '~' Left
        {  8,  0,  15,  7 }, // '~' Right
        { 16,  0,  23,  7 }, // '*' Snake Body 1
        { 24,  0,  31,  7 }, // '*' Snake Body 2
        { 16, 16,  23, 23 }, // '+' Snake Food
        {  0, 32,   7, 39 }, // ' ' Empty
        { 24, 16,  31, 23 }, // 'X' Game Over tile
        { 0,  56, 104, 99 }, // Logo
        { 56,  5,  64, 18 }, // Digit 0
        { 72,  5,  80, 18 }, // Digit 1
        { 88,  5,  96, 18 }, // Digit 2
        { 104, 5,  112, 18 }, // Digit 3
        { 120, 5,  127, 18 }, // Digit 4
        { 56,  21,  64, 34 }, // Digit 5
        { 72,  21,  80, 34 }, // Digit 6
        { 88,  21,  96, 34 }, // Digit 7
        { 104, 21,  112, 34 }, // Digit 8
        { 120, 21,  127, 34 }, // Digit 9
    };

    glClear(GL_COLOR_BUFFER_BIT);

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            int tileId = 0;
            int ch = board[y * cols + x];
            float quadXSize = 2 / (float)cols;
            float quadYSize = 2 / (float)(rows + logo);

            glColor3f(1.0, 1.0, 1.0);

            switch (ch) {
            case '#':
                tileId = 0;
                break;
            case '@':
                if (deltaX == 0 && deltaY == -1) tileId = 2;
                else if (deltaX == 0 && deltaY == 1) tileId = 1;
                else if (deltaX == -1 && deltaY == 0) tileId = 3;
                else if (deltaX == 1 && deltaY == 0) tileId = 4;
                else tileId = 2;
                break;
            case '~':
                // Dont display during a turn, it looks bad.
                if (lastDeltaX == deltaX && lastDeltaY == deltaY) {
                    if (deltaX == 0 && deltaY == -1) tileId = 6;
                    else if (deltaX == 0 && deltaY == 1) tileId = 5;
                    else if (deltaX == -1 && deltaY == 0) tileId = 7;
                    else if (deltaX == 1 && deltaY == 0) tileId = 8;
                    else tileId = 6;
                }
                else {
                    tileId = 12;
                }
                lastDeltaX = deltaX;
                lastDeltaY = deltaY;
                break;
            case '*':
                tileId = (x + y) % 2 ? 9 : 10;
                break;
            case '+':
                tileId = 11;
                break;
            case ' ':
                tileId = 12;
                break;
            case 'X':
                tileId = 13;
                break;
            }

            // Draw tile
            glBegin(GL_QUADS);
            glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
            glVertex3f(quadXSize * (x + 0) - 1, quadYSize * ((rows - y - 1) + 0) - 1, 0.0);
            glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
            glVertex3f(quadXSize * (x + 1) - 1, quadYSize * ((rows - y - 1) + 0) - 1, 0.0);
            glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
            glVertex3f(quadXSize * (x + 1) - 1, quadYSize * ((rows - y - 1) + 1) - 1, 0.0);
            glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
            glVertex3f(quadXSize * (x + 0) - 1, quadYSize * ((rows - y - 1) + 1) - 1, 0.0);
            glEnd();

        }
    }

    // Draw logo
    {
        int tileId = 14;
        glBegin(GL_QUADS);
        glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
        glVertex3f(-1, 1.0, 0);
        glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
        glVertex3f(1, 1.0, 0);
        glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
        glVertex3f(1, 0.51, 0);
        glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
        glVertex3f(-1, 0.51, 0);
        glEnd();
    }

    // Draw score
    {
        int i;
        int score = (sanp.length - 1) * 100; // Changed snake to sanp
        for (i = 0; i < 5; i++) {
            int digit = score - ((score / 10) * 10);
            int tileId = 15 + digit;
            score /= 10;
            glBegin(GL_QUADS);
            glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
            glVertex3f(0.05 + (4 - i) * 0.17, 0.75, 0);
            glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y1) / (float)snakeTileSet.height);
            glVertex3f(0.20 + (4 - i) * 0.17, 0.75, 0);
            glTexCoord2f(tiles[tileId].x2 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
            glVertex3f(0.20 + (4 - i) * 0.17, 0.60, 0);
            glTexCoord2f(tiles[tileId].x1 / (float)snakeTileSet.width, (tiles[tileId].y2) / (float)snakeTileSet.height);
            glVertex3f(0.05 + (4 - i) * 0.17, 0.60, 0);
            glEnd();
        }
    }


    glFlush();

    glutSwapBuffers();
}

void setup() {
    deltaX = 0;
    deltaY = 0;
    isGameOver = 0;
    gameOverTimeout = 60;
    setup_food();
    sanp_setter();
    fill_board();
    draw_food();
    draw_snake();
}

void idleFunc() {

    if (isGameOver) {

        usleep(1000000 / 10.0);

        gameOverTimeout--;
        if (gameOverTimeout == 0) {
            setup();
        }
        fill_board();
        draw_food();
        draw_snake();

    }
    else {

        usleep(2550000 / (7 + (sanp.length / (float)foods) * 10.0));

        fill_board();
        draw_food();
        draw_snake();
        game_rules();
        snake_move(deltaX, deltaY);
    }

    glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y) {

    switch (tolower(key)) {
    case 'w': deltaX = 0; deltaY = -1; break; // Move up
    case 's': deltaX = 0; deltaY = 1; break; // Move down
    case 'a': deltaX = -1; deltaY = 0; break;// Move left
    case 'd': deltaX = 1; deltaY = 0; break; // Move right
    }
    switch (key) {
    case 'r': setup(); break;
    }
}

void specialFunc(int key, int x, int y) {
    if (!isGameOver) {
        switch (key) {
        case GLUT_KEY_UP:    keyboardFunc('w', x, y); break;
        case GLUT_KEY_DOWN:  keyboardFunc('s', x, y); break;
        case GLUT_KEY_LEFT:  keyboardFunc('a', x, y); break;
        case GLUT_KEY_RIGHT: keyboardFunc('d', x, y); break;
        }
    }
}

void init() {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, snakeTileSet.width, snakeTileSet.height, 0, GL_RGB, GL_UNSIGNED_BYTE, snakeTileSet.pixel_data);
}

int main(int argc, char** argv)
{
    srand(time(0));//seeding

    setup();

    glutInit(&argc, argv);// Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);//how the window will be displayed, double buffering avoid flickering and tearing when rendering, RGB color mode for the window
    glutInitWindowSize(cols * 16, (rows + logo) * 16); //sets the initial width and height of the window to be created.
    glutCreateWindow("Danger rope food hope");//to create a new OpenGL window

    init(); //to initialize OpenGL settings and states that are required before rendering begins 

    glutDisplayFunc(displayFunc);//sets the display callback function for the current window, the displayfunc tells how to render
    glutKeyboardFunc(keyboardFunc);//set a keyboard callback function that handles keyboard input for the current window
    glutSpecialFunc(specialFunc);//callback function to handle special keyboard events, such as arrow keys and function keys
    glutIdleFunc(idleFunc);//idle callback is called repeatedly when no other events are being processed

    glutMainLoop();//loop continues indefinitely until the program terminates

    glDeleteTextures(1, &textureId);//to delete texture objects that are no longer needed

    return 0;
}
