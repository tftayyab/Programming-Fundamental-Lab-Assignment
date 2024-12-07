#include <stdio.h>
#include <GL/glut.h>
#include <string.h>

#define MAX_NAME_LENGTH 20
#define MAX_SCORES 1000

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} HighScore;

char playerName[MAX_NAME_LENGTH] = ""; // Buffer for the player's name
int nameLength = 0; // Current length of the name

HighScore highScores[MAX_SCORES];
int totalScores = 0;

// Function to render text
void renderBitmapText(float x, float y, const char* string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string);
        string++;
    }
}

void loadHighScores() {
    FILE* file = fopen("D:/Highscores.txt", "r");

    while (fscanf(file, "Name  = %s Score = %d\n", highScores[totalScores].name, &highScores[totalScores].score) != EOF) {
        totalScores++;
        if (totalScores >= MAX_SCORES) {
            break;  // Avoid exceeding the array size
        }
    }

    fclose(file);
}

/* void renderStrokeText(float x, float y, const char *string, void* font, float scale) { //to change size
    glPushMatrix();
    glTranslatef(x, y, 0.0f); // Move to the position
    glScalef(scale, scale, scale);
    while (*string) {
        glutStrokeCharacter(font, *string); // Render each character
        string++;
    }

    glPopMatrix();
} */

int compareScores(const void* a, const void* b) {
    // Cast the void pointers to HighScore pointers
    HighScore scoreA = *((HighScore*)a);
    HighScore scoreB = *((HighScore*)b);

    // Compare scores in descending order (higher score first)
    if (scoreA.score < scoreB.score) {
        return 1;
    }
    else if (scoreA.score > scoreB.score) {
        return -1;
    }
    else {
        return 0;  // Return 0 if both scores are equal
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //   renderStrokeText(-0.4f, 0.7f, "SNAKE GAME", GLUT_STROKE_ROMAN, 0.001f);  //LARGe Text
    glColor3f(1.0f, 1.0f, 1.0f);  //changes color for nect lines
    renderBitmapText(-0.2f, 0.7f, "SNAKE GAME");

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapText(-0.6f, 0.4f, "Enter Your Name:");

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapText(-0.6f, 0.3f, playerName);

    // Render Top 5 High Scores

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapText(-0.6f, 0.0f, "Top 5 High Scores:");

    loadHighScores();
    qsort(highScores, totalScores, sizeof(HighScore), compareScores); //sorts the high score

    //print top 5 scores
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 5; i++) {
        char scoreText[50];
        sprintf(scoreText, "%d. %s - %d", i + 1, highScores[i].name, highScores[i].score);
        renderBitmapText(-0.6f, -0.1f - i * 0.1f, scoreText);
    }

    glutSwapBuffers();
}

void storename() { //call after the player enters name
    FILE* file;
    file = fopen("D:/Highscores.txt", "a");

    fprintf(file, "Name  = %s ", playerName);

    fclose(file);

}

/* void storescore() { //call after the score is calculated
    FILE* file;
    file = fopen("D:/Highscores.txt", "a");

    fprintf(file, "Score = %d\n", score);

    fclose(file);

} */

void keyboard(unsigned char key, int x, int y) { //Keyboard Input
    if (key == '\r' || key == '\n') { // Enter key to finish input
        printf("Player Name Entered: %s\n", playerName);
        storename();
        return;
    }

    if (key == '\b' || key == 127) { // Backspace key to remove a character
        if (nameLength > 0) {
            playerName[--nameLength] = '\0';
        }
    }
    else if (nameLength < MAX_NAME_LENGTH - 1) { // Regular character input
        playerName[nameLength++] = key;
        playerName[nameLength] = '\0';
    }

    glutPostRedisplay(); // Refresh the screen to display the updated name
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glColor3f(1.0, 1.0, 1.0);         // White text
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set coordinate system
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Snake Game Intro");

    init();

    glutKeyboardFunc(keyboard); // Register keyboard input handle
    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
