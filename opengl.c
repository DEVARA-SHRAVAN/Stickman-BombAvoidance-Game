#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_BOMBS 10
#define MAX_SCORES 5

float stickmanX = 0.0f, stickmanY = -0.5f;
float bombX[MAX_BOMBS], bombY[MAX_BOMBS];
bool isJumping = false, isBombFalling = false;
bool isStickmanVisible = true;
bool isGameOver = false;

int score = 0;
int previousScores[MAX_SCORES] = {0};
int scoreCount = 0;

void drawText(float x, float y, char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string++);
    }
}

void drawStickman() {
    glPushMatrix();
    glTranslatef(stickmanX, stickmanY + (isJumping ? 0.3f : 0.0f), 0.0f);
    glColor3f(0, 0, 1);
    glutSolidSphere(0.05f, 20, 20);  // head
    glBegin(GL_LINES); // body and limbs
        glVertex2f(0, -0.05f); glVertex2f(0, -0.2f); // body
        glVertex2f(-0.05f, -0.1f); glVertex2f(0.05f, -0.1f); // arms
        glVertex2f(0, -0.2f); glVertex2f(-0.05f, -0.3f); // left leg
        glVertex2f(0, -0.2f); glVertex2f(0.05f, -0.3f);  // right leg
    glEnd();
    glPopMatrix();
}

void drawBomb(float x, float y) {
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glutSolidSphere(0.03f, 15, 15);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (isStickmanVisible) drawStickman();

    if (isBombFalling) {
        for (int i = 0; i < MAX_BOMBS; i++) {
            drawBomb(bombX[i], bombY[i]);
        }
    }

    char scoreStr[30];
    sprintf(scoreStr, "Score: %d", score);
    drawText(-0.95f, 0.9f, scoreStr);

    if (isGameOver) {
        drawText(-0.2f, 0.0f, "Game Over! Press 'r' to Restart");
        drawText(-0.2f, -0.1f, "Previous Scores:");
        for (int i = 0; i < scoreCount; i++) {
            char s[30];
            sprintf(s, "%d", previousScores[i]);
            drawText(-0.2f + (i * 0.15f), -0.2f, s);
        }
    }

    glutSwapBuffers();
}

void update(int value) {
    if (isJumping) {
        stickmanY += 0.02f;
        if (stickmanY >= 0.2f) isJumping = false;
    } else if (stickmanY > -0.5f) {
        stickmanY -= 0.02f;
    }

    if (isBombFalling && !isGameOver) {
        for (int i = 0; i < MAX_BOMBS; i++) {
            bombY[i] -= 0.02f;
            if (bombY[i] < -1.0f) {
                bombX[i] = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
                bombY[i] = 1.0f;
                score++;
            }

            if (fabs(bombX[i] - stickmanX) < 0.08f && fabs(bombY[i] - stickmanY) < 0.15f) {
                isGameOver = true;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(20, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (isGameOver && key != 'r') return;

    switch (key) {
        case 'c': isStickmanVisible = true; break;
        case 'a': stickmanX -= 0.1f; break;
        case 'd': stickmanX += 0.1f; break;
        case 'w': isJumping = true; break;
        case 'b':
            isBombFalling = true;
            for (int i = 0; i < MAX_BOMBS; i++) {
                bombX[i] = -1.0f + 2.0f * ((float)rand() / RAND_MAX);
                bombY[i] = 1.0f;
            }
            break;
        case 'r':
            if (scoreCount < MAX_SCORES) previousScores[scoreCount++] = score;
            isGameOver = false;
            score = 0;
            stickmanX = 0.0f;
            stickmanY = -0.5f;
            isJumping = false;
            isBombFalling = false;
            break;
    }
    glutPostRedisplay();
}

void menu(int option) {
    if (option == 1) exit(0);
}

void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Stickman Bomb Avoidance Game");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(20, update, 0);

    glutCreateMenu(menu);
    glutAddMenuEntry("Exit", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}
