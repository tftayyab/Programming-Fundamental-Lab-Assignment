#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <GL/glut.h>

#define cols 25
#define rows 25
#define foods 50

char board[cols * rows];

int isGameOver = 0;

void fill_board() {  // Tayyab: Fills the game board with borders and empty spaces
    int x, y;

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1) {
                board[y * cols + x] = '#';  // Border
            } else {
                board[y * cols + x] = isGameOver ? 'X' : ' ';  // Empty space
            }
        }
    }
}

/* void clear_screen() {  // Essa: Clears the console screen
    system("cls");  // Windows-specific screen clear
} */

/* void board_printer() {  // Essa: Prints the current game board
    int x, y;

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            putchar(board[y * cols + x]);
        }
        putchar('\n');
    }
} */

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

void draw_snake() {  // Tayyab: Draws the snake on the board based on its position
    int i;

    for (i = sanp.length - 1; i > 0; i--) {
        board[sanp.part[i].y * cols + sanp.part[i].x] = '*';  // Snake body
    }
    board[sanp.part[0].y * cols + sanp.part[0].x] = '@';  // Snake head
}

void snake_move(int deltaX, int deltaY) {  // Essa: Moves the snake by updating its coordinates
    int i;

    for (i = sanp.length - 1; i > 0; i--) {
        sanp.part[i] = sanp.part[i - 1];  // Shift the snake's body
    }

    sanp.part[0].x += deltaX;  // Move head in X direction
    sanp.part[0].y += deltaY;  // Move head in Y direction
}

/* void keyboard_reader() {  // Essa: Reads the key press and moves the snake accordingly
    int ch = getch();

    switch (tolower(ch)) {
        case 'w': snake_move(0, -1); break;  // Move up
        case 's': snake_move(0, 1); break;   // Move down
        case 'a': snake_move(-1, 0); break;  // Move left
        case 'd': snake_move(1, 0); break;   // Move right
    }
} */

void draw_food() {  // Hanzala: Draws food on the board if it has not been consumed
    int i;

    for (i = 0; i < foods; i++) {
        if (!food[i].consumed) {
            board[food[i].y * cols + food[i].x] = '+';  // Food
        }
    }
}

void setup_food() {  // Hanzala: Sets up the food positions randomly on the board
    int i;

    for (i = 0; i < foods; i++) {
        food[i].x = 1 + rand() % (cols - 2);
        food[i].y = 1 + rand() % (rows - 2);
        food[i].consumed = 0;  // Food is initially not consumed
    }
}

void sanp_setter() {  // Tayyab: Initializes the snake with a random position and length of 1
    sanp.length = 1;
    sanp.part[0].x = 1 + rand() % (cols - 2);
    sanp.part[0].y = 1 + rand() % (rows - 2);
}

void game_rules() {  // Hanzala: Checks the game rules like collision and food consumption
    int i;

    for (i = 0; i < foods; i++) {
        if (!food[i].consumed) {
            if (food[i].x == sanp.part[0].x && food[i].y == sanp.part[0].y) {
                food[i].consumed = 1;  // Food consumed
                sanp.length++;  // Increase snake length
            }
        }
    }

    if (sanp.part[0].x == 0 || sanp.part[0].x == cols - 1 ||
        sanp.part[0].y == 0 || sanp.part[0].y == rows - 1) {
        isGameOver = 1;  // Game over if snake hits a wall
    }

    for (i = 1; i < sanp.length; i++) {
        if (sanp.part[0].x == sanp.part[i].x && sanp.part[0].y == sanp.part[i].y) {
            isGameOver = 1;  // Game over if snake collides with itself
        }
    }
}

void displayFunc() {

    int y,x;

    glClear(GL_COLOR_BUFFER_BIT);


    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            int ch = (board[y * cols + x]);
            float quadXSize = 2 / (float)cols;
            float quadYSize = 2 / (float)rows;
            
            switch(ch) {
                case '#': glColor3f( 0.5, 0.5, 0.5 ); break;
                case '@': glColor3f( 1.0, 0.0, 0.0 ); break;
                case '*': glColor3f( 1.0, 0.0, 0.0 ); break;
                case '+': glColor3f( 0.0, 1.0, 0.0 ); break;
                case ' ': glColor3f( 0.0, 0.0, 0.0 ); break;
                case 'X': glColor3f( 1.0, 0.0, 1.0 ); break; //Game Over
            }

            glBegin(GL_QUADS);
            glVertex3f( quadXSize * (x+0) - 1, quadYSize * ((rows-y-1)+0) - 1, 0.0);
            glVertex3f( quadXSize * (x+1) - 1, quadYSize * ((rows-y-1)+0) - 1, 0.0);
            glVertex3f( quadXSize * (x+1) - 1, quadYSize * ((rows-y-1)+1) - 1, 0.0);
            glVertex3f( quadXSize * (x+0) - 1, quadYSize * ((rows-y-1)+1) - 1, 0.0);
            glEnd();

        }
        putchar('\n');
    }

    glutSwapBuffers();

}

int deltaX = 0, deltaY = 0;

void keyboardFunc(unsigned char key, int x, int y) {

    switch (tolower(key)) {
        case 'w': deltaX =  0; deltaY = -1; break;  // Move up
        case 's': deltaX =  0; deltaY =  1; break;   // Move down
        case 'a': deltaX = -1; deltaY =  0; break;  // Move left
        case 'd': deltaX =  1; deltaY =  0; break;   // Move right
    }

}

void idleFunc() {

    usleep(100000);

    fill_board();
    draw_food();
    draw_snake();
    game_rules();

    if (!isGameOver) {
        snake_move(deltaX, deltaY);
    }

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    srand(time(0));

    sanp_setter();
    setup_food();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Snake Game");

    glutDisplayFunc(displayFunc);
    glutKeyboardFunc(keyboardFunc);
    glutIdleFunc(idleFunc);

    glutMainLoop();

    return 0;
}