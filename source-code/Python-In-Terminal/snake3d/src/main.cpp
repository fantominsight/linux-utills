#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <cstdio>

// ============== GAME CONFIG ==============
constexpr int GRID_W = 20;
constexpr int GRID_H = 20;
constexpr float CELL = 0.9f;
constexpr float MOVE_INTERVAL = 0.15f;

// ============== GAME STATE ==============
enum Dir { D_UP, D_DOWN, D_LEFT, D_RIGHT };
struct Seg { int x, y; };

std::vector<Seg> snake;
Seg food;
Dir dir = D_RIGHT, nextDir = D_RIGHT;
bool gameOver = false, paused = false, win = false;
int score = 0;
float moveTimer = 0;

// ============== CAMERA ==============
float camAngle = 0, camHeight = 18, camDist = 22;

// ============== HELPERS ==============
int randint(int mn, int mx) { return mn + rand() % (mx - mn + 1); }

bool occupied(int x, int y) {
    for (auto &s : snake) if (s.x == x && s.y == y) return true;
    return false;
}

void spawnFood() {
    do {
        food.x = randint(1, GRID_W - 2);
        food.y = randint(1, GRID_H - 2);
    } while (occupied(food.x, food.y));
}

void initGame() {
    srand(time(nullptr));
    snake.clear();
    snake.push_back({GRID_W/2, GRID_H/2});
    snake.push_back({GRID_W/2-1, GRID_H/2});
    snake.push_back({GRID_W/2-2, GRID_H/2});
    dir = D_RIGHT; nextDir = D_RIGHT;
    score = 0; gameOver = false; win = false; moveTimer = 0;
    spawnFood();
}

void moveSnake() {
    if (gameOver || paused) return;

    dir = nextDir;
    Seg head = snake.front();
    Seg newHead = head;
    switch (dir) {
        case D_UP:    newHead.y++; break;
        case D_DOWN:  newHead.y--; break;
        case D_LEFT:  newHead.x--; break;
        case D_RIGHT: newHead.x++; break;
    }

    // wall collision
    if (newHead.x < 0 || newHead.x >= GRID_W || newHead.y < 0 || newHead.y >= GRID_H) {
        gameOver = true;
        return;
    }

    // self collision (skip tail because it will move)
    for (size_t i = 0; i < snake.size() - 1; ++i) {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
            gameOver = true;
            return;
        }
    }

    snake.insert(snake.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        score++;
        if ((int)snake.size() >= GRID_W * GRID_H) {
            win = true;
            gameOver = true;
            return;
        }
        spawnFood();
    } else {
        snake.pop_back();
    }
}

// ============== RENDERING ==============
void drawFloor() {
    glBegin(GL_QUADS);
    for (int x = 0; x < GRID_W; ++x) {
        for (int y = 0; y < GRID_H; ++y) {
            bool light = (x + y) % 2 == 0;
            if (light) glColor3f(0.25f, 0.25f, 0.3f);
            else       glColor3f(0.15f, 0.15f, 0.2f);
            float x0 = x - GRID_W/2.0f, y0 = y - GRID_H/2.0f;
            glVertex3f(x0,     0, y0);
            glVertex3f(x0+1,   0, y0);
            glVertex3f(x0+1,   0, y0+1);
            glVertex3f(x0,     0, y0+1);
        }
    }
    glEnd();
}

void drawCube(float cx, float cy, float cz, float s, float r, float g, float b) {
    float h = s / 2.0f;
    glColor3f(r*0.6f, g*0.6f, b*0.6f);

    // normals for lighting
    GLfloat verts[][3] = {
        {cx-h, cy-h, cz+h}, {cx+h, cy-h, cz+h}, {cx+h, cy+h, cz+h}, {cx-h, cy+h, cz+h}, // front
        {cx+h, cy-h, cz-h}, {cx-h, cy-h, cz-h}, {cx-h, cy+h, cz-h}, {cx+h, cy+h, cz-h}, // back
        {cx-h, cy-h, cz-h}, {cx-h, cy-h, cz+h}, {cx-h, cy+h, cz+h}, {cx-h, cy+h, cz-h}, // left
        {cx+h, cy-h, cz+h}, {cx+h, cy-h, cz-h}, {cx+h, cy+h, cz-h}, {cx+h, cy+h, cz+h}, // right
        {cx-h, cy-h, cz-h}, {cx+h, cy-h, cz-h}, {cx+h, cy-h, cz+h}, {cx-h, cy-h, cz+h}, // bottom
        {cx-h, cy+h, cz+h}, {cx+h, cy+h, cz+h}, {cx+h, cy+h, cz-h}, {cx-h, cy+h, cz-h}, // top
    };
    GLfloat norms[][3] = {
        {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1},
        {0,0,-1},{0,0,-1},{0,0,-1},{0,0,-1},
        {-1,0,0},{-1,0,0},{-1,0,0},{-1,0,0},
        {1,0,0},{1,0,0},{1,0,0},{1,0,0},
        {0,-1,0},{0,-1,0},{0,-1,0},{0,-1,0},
        {0,1,0},{0,1,0},{0,1,0},{0,1,0},
    };

    glBegin(GL_QUADS);
    for (int i = 0; i < 24; ++i) {
        float shade = (i / 4 == 4) ? 0.5f : (i / 4 == 5) ? 1.0f : 0.8f;
        glColor3f(r * shade, g * shade, b * shade);
        glNormal3fv(norms[i]);
        glVertex3fv(verts[i]);
    }
    glEnd();
}

void drawSnake() {
    int n = snake.size();
    for (int i = 0; i < n; ++i) {
        float t = (float)i / n;
        float r = 1.0f - t * 0.6f;
        float g = 0.3f + t * 0.5f;
        float b = 0.1f + t * 0.3f;
        float x = snake[i].x - GRID_W/2.0f;
        float z = snake[i].y - GRID_H/2.0f;
        drawCube(x, 0.45f, z, 0.85f, r, g, b);
    }
}

void drawFood() {
    float x = food.x - GRID_W/2.0f;
    float z = food.y - GRID_H/2.0f;
    float pulse = 0.8f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
    glPushMatrix();
    glTranslatef(x, 0.45f, z);
    glScalef(pulse, pulse, pulse);
    glColor3f(1, 0.2f, 0.2f);
    glutSolidSphere(0.4f, 16, 16);
    glPopMatrix();
}

void drawWalls() {
    glColor3f(0.4f, 0.4f, 0.55f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    float hw = GRID_W/2.0f, hh = GRID_H/2.0f;
    glVertex3f(-hw, 0, -hh);
    glVertex3f( hw, 0, -hh);
    glVertex3f( hw, 0,  hh);
    glVertex3f(-hw, 0,  hh);
    glEnd();
}

void drawUI() {
    // Projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    static char buf[64];
    snprintf(buf, sizeof(buf), "Score: %d", score);
    glColor3f(1, 1, 1);
    glRasterPos2i(10, glutGet(GLUT_WINDOW_HEIGHT) - 25);
    for (char *c = buf; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    if (gameOver) {
        const char *msg = win ? "YOU WIN!" : "GAME OVER";
        const char *sub = "Press SPACE to restart";
        glColor3f(1, 0.2f, 0.2f);
        glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH)/2 - 80, glutGet(GLUT_WINDOW_HEIGHT)/2 + 20);
        for (const char *c = msg; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        glColor3f(0.8f, 0.8f, 0.8f);
        glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH)/2 - 90, glutGet(GLUT_WINDOW_HEIGHT)/2 - 10);
        for (const char *c = sub; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    if (paused && !gameOver) {
        const char *msg = "PAUSED";
        glColor3f(1, 1, 0.2f);
        glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH)/2 - 40, glutGet(GLUT_WINDOW_HEIGHT)/2);
        for (const char *c = msg; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[]  = {0.3f, 0.3f, 0.4f, 1.0f};
    GLfloat diffuse[]  = {0.9f, 0.9f, 1.0f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat position[] = {10.0f, 20.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_NORMALIZE);
}

// ============== CALLBACKS ==============
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float cx = camDist * cos(camAngle);
    float cz = camDist * sin(camAngle);
    gluLookAt(cx, camHeight, cz, 0, 0, 0, 0, 1, 0);

    drawFloor();
    drawWalls();
    drawSnake();
    drawFood();
    drawUI();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)w/h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case ' ':
            if (gameOver) initGame();
            else paused = !paused;
            break;
        case 'r': case 'R':
            initGame();
            break;
    }
}

void specialKeys(int key, int, int) {
    if (gameOver || paused) return;
    switch (key) {
        case GLUT_KEY_UP:    if (dir != D_DOWN)  nextDir = D_UP;    break;
        case GLUT_KEY_DOWN:  if (dir != D_UP)    nextDir = D_DOWN;  break;
        case GLUT_KEY_LEFT:  if (dir != D_RIGHT) nextDir = D_LEFT;  break;
        case GLUT_KEY_RIGHT: if (dir != D_LEFT)  nextDir = D_RIGHT; break;
    }
}

void mouseMotion(int x, int y) {
    static int prevX = x, prevY = y;
    int dx = x - prevX, dy = y - prevY;
    prevX = x; prevY = y;
    camAngle += dx * 0.005f;
    camHeight -= dy * 0.05f;
    if (camHeight < 2) camHeight = 2;
    if (camHeight > 40) camHeight = 40;
    glutPostRedisplay();
}

void idle() {
    int dt = glutGet(GLUT_ELAPSED_TIME);
    static int lastTime = dt;
    float elapsed = (dt - lastTime) / 1000.0f;
    lastTime = dt;

    // auto-rotate when not interacting
    camAngle += 0.15f * elapsed;

    moveTimer += elapsed;
    if (moveTimer >= MOVE_INTERVAL) {
        moveTimer -= MOVE_INTERVAL;
        moveSnake();
    }

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("3D Snake");

    glClearColor(0.08f, 0.08f, 0.12f, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    setupLighting();

    initGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
