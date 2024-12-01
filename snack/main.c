#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <ncurses/ncurses.h>  

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
                board[y * cols + x] = ' ';  // Empty space
            }
        }
    }
}

void clear_screen() {  // Essa: Clears the console screen
   // system("cls");  // Windows-specific screen clear
   refresh();   // Changed to use 'refresh()' for ncurses screen update.
}

void board_printer() {  // Essa: Prints the current game board
    int x, y;

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            putch(board[y * cols + x]);
        }
        putch('\n');
    }
}

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

void keyboard_reader() {  // Essa: Reads the key press and moves the snake accordingly
	int ch = getch();
	static int deltaX = 0, deltaY = 0;  // Added static variables for persistent movement direction.
	
    switch (tolower(ch)) {
        case 'w': deltaX =  0; deltaY = -1; break;  // Move up
        case 's': deltaX =  0; deltaY =  1; break;   // Move down
        case 'a': deltaX = -1; deltaY =  0; break;  // Move left
        case 'd': deltaX =  1; deltaY =  0; break;   // Move right
    }
    
    snake_move(deltaX, deltaY); // Updated to use the persistent movement logic.
}

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

int main(int argc, char **argv) {
    srand(time(0));
	initscr();
	noecho();
	timeout(100);
	
    sanp_setter();
    setup_food();

    while (!isGameOver) {
    	move(0,0); 
        fill_board();
        draw_food();
        draw_snake();
        game_rules();
        clear_screen();
        move(cols,0);
        printw("Snake Game, Score: %d\n", sanp.length * 100);  //changed 'printf' to 'printw' bcz of ncurses library
        board_printer();
        if (!isGameOver) keyboard_reader();
    }
    
	move(cols,0);
    printw("Game Over, Final score: %d\n", sanp.length * 100);   //changed 'printf' to 'printw' bcz of ncurses library

    while (1) getch(); 

	endwin();

    return 0;
}
