#pragma comment (lib, "winmm.lib");
#include <windows.h>
#include <MMsystem.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
// Global variables
float shipX = 0.0f, shipY = 0.0f, shipZ = -15.0f;
float ship2X = 10.0f, ship2Y = 0.0f, ship2Z = -20.0f;
float truckX[5] = {-10.0f, 0.0f, 10.0f, -0.5f, 1.3f}, truckY = 0.56f, truckZ[5] = {-3.5f, -3.0f, -5.5f, -5.8f, 10.0f};
float carX[2] = {-2.0f, 2.0f}, carY = 0.56f, carZ[2] = {8.0f, 8.0f};
float workerX[8] = {-6.0f, -2.0f, 2.0f, 6.0f, -8.0f, 0.0f, 4.0f, 8.0f};
float workerZ[8] = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 3.0f, 2.0f, 3.0f};
bool gateOpen = false;
float workerSpeedX[8], workerSpeedZ[8];
float cameraAngleX = 0.0f, cameraAngleY = 0.5f, cameraDistance = 32.0f;
float minCameraDistance = 5.0f;  // Minimum zoom distance (prevent clipping)
float maxCameraDistance = 50.0f; // Maximum zoom distance (prevent losing sight)
float ropeAngle = 0.0f;
bool ropeSwing = true;
float craneAngle = 0.0f;
float doorAngle = 0.0f;
float targetDoorAngle = 0.0f;
float hookHeight = 0.0f;
bool craneWorking = false;
bool craneReturning = false;
int currentContainer = -1;
float containerPos[122][3];
float truckContainerPos[5][3] = {{-100, -100, -100}, {-100, -100, -100}, {-100, -100, -100}, {-100, -100, -100}, {-100, -100, -100}}; // Initialize all
int truckContainerColor[5] = {-1, -1, -1, -1, -1}; // Initialize all
int selectedShip = -1;
int selectedTruck = -1;
bool isShip1Anchored = false;
bool isShip2Anchored = false;
float waveTime = 0.0f;
bool ship1MovingForward = false;
bool ship2MovingForward = false;
bool containerGrabbed = false;
float containerX = 0.0f, containerY = 0.0f, containerZ = 0.0f;


bool ship1SoundPlaying = false;
bool ship2SoundPlaying = false;
bool craneSoundPlaying = false;
bool doorSoundPlayed = false;
bool isWaveSoundPlaying = false;



// Smoke particle structure
struct SmokeParticle {
    float x, y, z;      // Position
    float vx, vz;       // Velocity for slight horizontal drift
    float lifetime;     // Time remaining before particle resets
    bool active;        // Whether the particle is active
};

// Array of smoke particles (for each ship)
const int NUM_SMOKE_PARTICLES = 30; // Number of particles per ship
SmokeParticle smokeParticlesShip1[NUM_SMOKE_PARTICLES];
SmokeParticle smokeParticlesShip2[NUM_SMOKE_PARTICLES];
float smokeEmissionTimer = 0.0f;    // Timer for emitting new particles
float smokeEmissionRate = 0.2f;     // Emit a new particle every 0.2 seconds

// Colors
GLfloat waterColor[] = {0.1f, 0.3f, 0.6f};
GLfloat dockColor[] = {0.5f, 0.45f, 0.4f};
GLfloat shipHullColor[] = {0.15f, 0.15f, 0.2f};
GLfloat shipDeckColor[] = {0.7f, 0.7f, 0.75f};
GLfloat containerColors[][3] = {
    {0.8f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.8f}, {0.9f, 0.8f, 0.1f},
    {0.2f, 0.6f, 0.3f}, {0.8f, 0.5f, 0.2f}, {0.6f, 0.2f, 0.8f},
    {0.9f, 0.4f, 0.4f}, {0.4f, 0.4f, 0.9f}, {0.7f, 0.7f, 0.2f},
    {0.3f, 0.7f, 0.5f}, {0.5f, 0.3f, 0.7f}, {0.7f, 0.5f, 0.3f}
};
GLfloat warehouseColor[] = {0.6f, 0.6f, 0.65f};
GLfloat frontWallColor[] = {0.2f, 0.4f, 0.8f};
GLfloat backWallColor[] = {0.8f, 0.2f, 0.2f};
GLfloat leftWallColor[] = {0.2f, 0.8f, 0.2f};
GLfloat rightWallColor[] = {0.8f, 0.8f, 0.2f};
GLfloat pillarColor[] = {0.4f, 0.4f, 0.4f};
GLfloat ropeColor[] = {0.5f, 0.35f, 0.05f};
GLfloat craneColor[] = {0.9f, 0.7f, 0.1f};
GLfloat windowColor[] = {0.0f, 0.8f, 0.0f};
GLfloat bankColor[] = {0.2f, 0.6f, 0.2f};
GLfloat roadColor[] = {0.7f, 0.7f, 0.7f};
GLfloat trunkColor[] = {0.5f, 0.3f, 0.1f};
GLfloat foliageColor[] = {0.0f, 0.5f, 0.0f};
GLfloat coneColor[] = {1.0f, 0.5f, 0.0f};
GLfloat truckStopColor[] = {0.1f, 0.5f, 0.0f};

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    GLfloat lightPos[] = {10.0f, 15.0f, 10.0f, 1.0f};
    GLfloat lightAmb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiff[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    // Initialize container positions
    for (int i = 0; i < 15; i++) {
        containerPos[i][0] = -15.0f + i * (2.0f + 0.1f);
        containerPos[i][1] = 1.0f;
        containerPos[i][2] = 5.5f;
    }
    for (int i = 15; i < 34; i++) {
        containerPos[i][0] = -15.0f + (i - 15) * (1.5f + 0.1f);
        containerPos[i][1] = 2.2f;
        containerPos[i][2] = 5.5f;
    }
    for (int i = 34; i < 61; i++) {
        containerPos[i][0] = -15.0f + (i - 34) * (1.0f + 0.1f);
        containerPos[i][1] = 3.7f;
        containerPos[i][2] = 5.5f;
    }
    for (int i = 61; i < 76; i++) {
        containerPos[i][0] = -15.0f + (i - 61) * (2.0f + 0.1f);
        containerPos[i][1] = 1.0f;
        containerPos[i][2] = 4.0f;
    }
    for (int i = 76; i < 95; i++) {
        containerPos[i][0] = -15.0f + (i - 76) * (1.5f + 0.1f);
        containerPos[i][1] = 2.2f;
        containerPos[i][2] = 4.0f;
    }
    for (int i = 95; i < 122; i++) {
        containerPos[i][0] = -15.0f + (i - 95) * (1.0f + 0.1f);
        containerPos[i][1] = 3.7f;
        containerPos[i][2] = 4.0f;
    }

    // Initialize worker speeds
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 8; i++) {
        workerSpeedX[i] = (rand() % 100 - 50) / 5000.0f;
        workerSpeedZ[i] = (rand() % 100 - 50) / 5000.0f;
    }


    // Initialize smoke particles for both ships
    for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
        smokeParticlesShip1[i].active = false;
        smokeParticlesShip1[i].lifetime = 0.0f;

        smokeParticlesShip2[i].active = false;
        smokeParticlesShip2[i].lifetime = 0.0f;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void drawWater() {
    // Start playing wave sound in loop (if not already playing)
    if (!isWaveSoundPlaying) {
        PlaySound(TEXT("waves.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        isWaveSoundPlaying = true;
    }

    // --- Existing wave rendering code ---
    glDisable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    float waterColor[3] = {0.2f, 0.5f, 0.8f};
    glColor3fv(waterColor);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glBegin(GL_QUADS);
    for (float x = -50.0f; x < 50.0f; x += 1.0f) {
        for (float z = -50.0f; z < 50.0f; z += 1.0f) {
            float wave1 = 0.3f * sin(waveTime * 1.0f + x * 0.2f);
            float wave2 = 0.2f * cos(waveTime * 0.8f + z * 0.3f);
            float wave3 = 0.1f * sin(waveTime * 1.5f + (x + z) * 0.1f);
            float y = wave1 + wave2 + wave3-0.5f;

            float nx = -0.2f * cos(waveTime * 1.0f + x * 0.2f);
            float nz = -0.3f * sin(waveTime * 0.8f + z * 0.3f);
            float ny = 1.0f;

            glNormal3f(nx, ny, nz);
            glVertex3f(x, y, z);
            glVertex3f(x + 1.0f, y, z);
            glVertex3f(x + 1.0f, y, z + 1.0f);
            glVertex3f(x, y, z + 1.0f);
        }
    }
    glEnd();

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    // --- End of existing code ---
}

// Add this to your cleanup function (to stop sound when closing)
void cleanup() {
    PlaySound(NULL, NULL, 0);  // Stop any playing sound
    isWaveSoundPlaying = false;
}

void drawPier() {
    glColor3fv(dockColor);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(30.0f, 0.2f, 15.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 7.5f);
    glScalef(30.0f, 0.3f, 0.3f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -7.5f);
    glScalef(30.0f, 0.3f, 0.3f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawPillar(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3fv(pillarColor);

    glPushMatrix();
    glTranslatef(0.0f, 1.0f, -1.5f);
    glScalef(0.6f, 1.5f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 1.8f, 0.0f);
    glTranslatef(0.0f, 0.2f, -1.5f);
    glutSolidSphere(0.3f, 10, 10);
    glPopMatrix();
    glPopMatrix();
}

void drawSupportPillar(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3fv(pillarColor);
    glPushMatrix();
    glTranslatef(0.0f, 2.25f, 0.0f);
    glScalef(0.8f, 4.5f, 0.8f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 4.7f, 0.0f);
    glutSolidSphere(0.4f, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, (5.5f - z) / 2);
    glScalef(0.2f, 0.2f, 1.7f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, -(z - 4.0f) / 2);
    glScalef(0.2f, 0.2f, 1.7f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
}

void drawTruckStop() {
    glPushMatrix();
    glTranslatef(0.3f, 0.2f, -5.8f);
    glColor3fv(truckStopColor);
    glScalef(4.5f, 0.1f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawDockCrane(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3fv(craneColor);
    glPushMatrix();
    glTranslatef(0.0f, 4.35f, -0.8f);
    glScalef(1.2f, 8.5f, 1.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 8.6f, -0.8f);
    glRotatef(craneAngle, 0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(18.0f, 0.3f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex3f(9.0f, 0.0f, 0.0f);
    glVertex3f(9.0f, -5.0f - hookHeight, 0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(9.0f, -5.0f - hookHeight, 0.0f);
    glutSolidSphere(0.2f, 5, 10);
    if (containerGrabbed) {
        glColor3fv(containerColors[currentContainer % 12]);
        glTranslatef(0.0f, 0.0f, 0.0f);
        glScalef(0.8f, 0.8f, 0.6f);
        glutSolidCube(1.0);
    }
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void drawRopes(float shipX, float shipZ, bool isAnchored) {
    if (isAnchored) {
        glColor3fv(ropeColor);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(shipX, 1.5f, shipZ - 2.0f);
        glVertex3f(-1.0f, 1.8f, -5.5f);
        glVertex3f(shipX, 1.5f, shipZ + 2.0f);
        glVertex3f(1.0f, 1.8f, -5.5f);
        glEnd();
        glPushMatrix();
        glTranslatef(shipX, 0.0f, shipZ - 2.0f);
        glRotatef(ropeAngle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
        glEnd();
        glPopMatrix();
    }
}

void drawShip(float shipX, float shipY, float shipZ, int shipIndex) {
    glPushMatrix();
    glTranslatef(shipX, shipY, shipZ);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glColor3fv(shipHullColor);
    glPushMatrix();
    glScalef(10.0f, 3.0f, 3.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-5.0f, 1.5f, 0.0f);
    glScalef(1.0f, 1.0f, 2.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 3.14159f / 20.0f;
        glVertex3f(cos(angle) * 1.0f, 0.0f, sin(angle) * 0.5f - 0.5f);
    }
    glEnd();
    glPopMatrix();
    glColor3fv(shipDeckColor);
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glScalef(8.0f, 0.2f, 2.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    glScalef(6.0f, 0.2f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3fv(shipDeckColor);
    glPushMatrix();
    glTranslatef(4.0f, 2.5f, 0.0f);
    glScalef(3.0f, 2.5f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(4.0f, 4.0f, 0.0f);
    glScalef(2.0f, 1.5f, 1.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    for (int i = 0; i < 20; i++) {
        if (!craneWorking || currentContainer != i) {
            glPushMatrix();
            float x = -4.0f + (i % 5) * 1.6f;
            float y = 1.7f + (i / 5) * 1.0f;
            float z = 0.0f;
            glTranslatef(x, y, z);
            glScalef(1.5f, 1.0f, 0.8f);
            glColor3fv(containerColors[i % 12]);
            glutSolidCube(1.0);
            glPopMatrix();
        }
    }
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(-4.0f, 2.5f, 0.0f);
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.2f, 4.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);

    glPushMatrix();
    glTranslatef(4.5f, 4.5f, 0.0f);
    glScalef(0.4f, 2.3f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw smoke particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    SmokeParticle* smokeParticles = (shipIndex == 0) ? smokeParticlesShip1 : smokeParticlesShip2;
    for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
        if (smokeParticles[i].active) {
            glPushMatrix();
            glTranslatef(smokeParticles[i].x, smokeParticles[i].y, smokeParticles[i].z);
            float alpha = smokeParticles[i].lifetime / 3.0f; // Fade from 1 to 0
            glColor4f(0.2f, 0.2f, 0.2f, alpha); // Dark gray with alpha
            float size = 0.2f + (1.0f - alpha) * 0.3f; // Grow as it fades
            glutSolidSphere(size, 8, 8);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);

    glPopMatrix();
}
void drawTruck(int index) {
    glPushMatrix();
    glTranslatef(truckX[index], truckY, truckZ[index]);
    if (index == 4) {
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    }
    if (index == 3) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    glColor3f(index == 0 ? 0.8f : (index == 1 ? 0.8f : (index == 2 ? 0.1f : (index == 3 ? 0.2f : 0.6f))),
              index == 0 ? 0.1f : (index == 1 ? 0.3f : (index == 2 ? 0.1f : (index == 3 ? 0.8f : 0.2f))),
              index == 0 ? 0.1f : (index == 1 ? 0.1f : (index == 2 ? 0.8f : (index == 3 ? 0.2f : 0.8f))));
    glPushMatrix();
    glTranslatef(0.7f, 0.7f, 0.0f);
    glScalef(1.4f, 1.2f, 1.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(-1.2f, 0.5f, 0.0f);
    glScalef(2.5f, 1.0f, 1.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    for (float x : {0.7f, -0.8f, -2.2f}) {
        for (float z : {-0.5f, 0.5f}) {
            glPushMatrix();
            glTranslatef(x, 0.0f, z);
            glutSolidTorus(0.1f, 0.25f, 10, 10);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void drawCar(int index) {
    glPushMatrix();
    glTranslatef(carX[index], carY, carZ[index]);
    glColor3f(index == 0 ? 0.9f : 0.1f, index == 0 ? 0.9f : 0.1f, index == 0 ? 0.9f : 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glScalef(1.0f, 0.8f, 0.8f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    for (float x : {-0.3f, 0.3f}) {
        for (float z : {-0.3f, 0.3f}) {
            glPushMatrix();
            glTranslatef(x, 0.0f, z);
            glutSolidTorus(0.05f, 0.15f, 10, 10);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void drawContainer(float x, float y, float z, const GLfloat* color, int index) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3fv(color);
    // Use fixed scale for containers on trucks for consistency
    float scale = (index >= 0 && index < 122) ?
                  ((index < 15 || (index >= 61 && index < 76)) ? 2.0f :
                   (index < 34 || (index >= 76 && index < 95)) ? 1.5f : 1.0f) : 1.0f;
    glScalef(scale, 1.0f, 0.8f);
    glutSolidCube(1.0);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.7f, -0.4f, 0.41f); glVertex3f(0.7f, -0.4f, 0.41f);
    glVertex3f(-0.7f, 0.0f, 0.41f); glVertex3f(0.7f, 0.0f, 0.41f);
    glVertex3f(-0.7f, 0.4f, 0.41f); glVertex3f(0.7f, 0.4f, 0.41f);
    glEnd();
    glPopMatrix();
}

void drawWorker(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.8f, 0.6f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glutSolidSphere(0.15f, 10, 10);
    glPopMatrix();
    glColor3f(0.0f, 0.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glScalef(0.3f, 0.5f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);
    for (float dx : {-0.1f, 0.1f}) {
        glPushMatrix();
        glTranslatef(dx, 0.1f, 0.0f);
        glScalef(0.1f, 0.4f, 0.1f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glColor3f(0.8f, 0.6f, 0.4f);
    for (float dx : {-0.2f, 0.2f}) {
        glPushMatrix();
        glTranslatef(dx, 0.5f, 0.0f);
        glScalef(0.1f, 0.3f, 0.1f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.8f, 0.0f);
    glutSolidSphere(0.18f, 10, 10);
    glPopMatrix();
    glPopMatrix();
}

void drawCone(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3fv(coneColor);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.1f, 0.15f, -0.1f);
    glVertex3f(-0.1f, 0.15f, 0.1f);
    glVertex3f(0.1f, 0.15f, 0.1f);
    glVertex3f(0.1f, 0.15f, -0.1f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.1f, 0.05f, -0.1f);
    glVertex3f(-0.1f, 0.05f, 0.1f);
    glVertex3f(0.1f, 0.05f, 0.1f);
    glVertex3f(0.1f, 0.05f, -0.1f);
    glEnd();
    glPopMatrix();
}

void drawWarehouse() {
    glPushMatrix();
    glTranslatef(11.0f, 0.0f, 0.0f);
    glColor3fv(frontWallColor);

    glPushMatrix();
    glTranslatef(0.0f, 2.5f, -3.1f);
    glScalef(10.0f, 5.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3fv(backWallColor);
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, 3.1f);
    glScalef(10.0f, 5.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3fv(leftWallColor);
    glPushMatrix();
    glTranslatef(-5.0f, 2.5f, 0.0f);
    glScalef(0.2f, 5.0f, 6.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3fv(rightWallColor);
    glPushMatrix();
    glTranslatef(5.0f, 2.5f, 0.0f);
    glScalef(0.2f, 5.0f, 6.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw interior space (white quad) when door is open
    if (gateOpen) {
        glPushMatrix();
        glTranslatef(-4.85f, 2.0f, 0.0f); // Moved forward to avoid left wall
        glTranslatef(0.0f, 0.0f, 1.5f); // Align with door pivot
        glColor3f(1.0f, 1.0f, 1.0f); // White for interior
        glDisable(GL_CULL_FACE); // Ensure visible from both sides
        // Temporarily disable depth testing to debug visibility
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_QUADS);
        glVertex3f(0.0f, 2.0f, -1.5f); // Top-left
        glVertex3f(0.0f, 2.0f, 1.5f);  // Top-right
        glVertex3f(0.0f, -2.0f, 1.5f); // Bottom-right
        glVertex3f(0.0f, -2.0f, -1.5f); // Bottom-left
        glEnd();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glPopMatrix();
    }

    // Draw the door
    glPushMatrix();
    glTranslatef(-5.0f, 2.0f, 0.0f); // Adjusted for larger door
    glRotatef(doorAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 1.5f); // Pivot adjusted for larger door
    glColor3f(0.4f, 0.5f, 0.5f); // Door color
    glDisable(GL_CULL_FACE); // Disable culling to ensure door is visible when rotated
    glScalef(0.5f, 4.0f, 3.0f); // Increased door size
    glutSolidCube(1.0);
    glEnable(GL_CULL_FACE); // Re-enable culling

    // Draw door handle (small cube)
    glColor3f(0.9f, 0.9f, 0.59f); // Silver/gray handle
    glPushMatrix();
    glTranslatef(0.26f, 0.0f, 0.55f); // Position handle on outer face
    glScalef(0.1f, 0.2f, 0.1f); // Small handle size
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw door outline for detail (optional)
    glColor3f(0.0f, 0.0f, 0.0f); // Black outline
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.26f, 0.8f, -0.5f);
    glVertex3f(0.26f, 0.8f, 0.5f);
    glVertex3f(0.26f, -0.8f, 0.5f);
    glVertex3f(0.26f, -0.8f, -0.5f);
    glEnd();
    glPopMatrix();

    glColor3fv(windowColor);
    glPolygonOffset(-1.0f, -1.0f);
    for (int i = -3; i <= 3; i += 2) {
        glPushMatrix();
        glTranslatef(i * 1.5f, 3.5f, -3.15f);
        glScalef(0.75f, 0.75f, 0.1f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    for (int i = -1; i <= 1; i += 2) {
        for (int j = 0; j <= 1; j++) {
            glPushMatrix();
            glTranslatef(i * 2.0f, 1.5f + j * 1.5f, 3.15f);
            glScalef(0.75f, 0.75f, 0.1f);
            glutSolidCube(1.0);
            glPopMatrix();
        }
    }
    for (int i = -2; i <= 2; i += 2) {
        glPushMatrix();
        glTranslatef(-5.05f, 2.5f, i * 1.0f);
        glScalef(0.1f, 0.75f, 0.75f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    for (int i = -2; i <= 2; i += 2) {
        glPushMatrix();
        glTranslatef(5.05f, 2.5f, i * 1.0f);
        glScalef(0.1f, 0.75f, 0.75f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 5.0f, 0.0f);
    glScalef(10.5f, 0.5f, 6.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
}

void drawSeaBank() {
    glColor3fv(bankColor);
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 8.0f);
    glScalef(30.0f, 0.5f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawRoads() {
    glColor3fv(roadColor);
    glPushMatrix();
    glTranslatef(0.0f, 0.51f, 8.0f);
    glScalef(30.0f, 0.02f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.51f, 11.5f);
    glScalef(4.0f, 0.02f, 3.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float z : {6.5f, 7.5f, 8.5f}) {
        for (float x = -15.0f; x < 15.0f; x += 1.0f) {
            glBegin(GL_LINES);
            glVertex3f(x, 0.52f, z);
            glVertex3f(x + 0.5f, 0.52f, z);
            glEnd();
        }
    }
    for (float x : {-1.5f, -0.5f, 0.5f}) {
        for (float z = 10.0f; z < 13.0f; z += 1.0f) {
            glBegin(GL_LINES);
            glVertex3f(x, 0.52f, z);
            glVertex3f(x, 0.52f, z + 0.5f);
            glEnd();
        }
    }
}

void drawTree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glColor3fv(trunkColor);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.2f, 1.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3fv(foliageColor);
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glutSolidSphere(0.5f, 10, 10);
    glPopMatrix();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    float camX = cameraDistance * sin(cameraAngleX) * cos(cameraAngleY);
    float camY = cameraDistance * sin(cameraAngleY);
    float camZ = cameraDistance * cos(cameraAngleX) * cos(cameraAngleY);
    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    drawWater();
    drawPier();
    drawPillar(-1.0f, -5.5f);
    drawPillar(1.0f, -5.5f);
    drawTruckStop();
    drawSupportPillar(-10.0f, 4.75f);
    drawSupportPillar(-5.0f, 4.75f);
    drawSupportPillar(0.0f, 4.75f);
    drawSupportPillar(5.0f, 4.75f);
    drawSupportPillar(10.0f, 4.75f);
    drawDockCrane(-8.0f, -5.0f);
    drawRopes(shipX, shipZ, isShip1Anchored);
    drawRopes(ship2X, ship2Z, isShip2Anchored);
    drawShip(shipX, shipY, shipZ, 0);    // Ship 1 (index 0)
    drawShip(ship2X, ship2Y, ship2Z, 1); // Ship 2 (index 1)
    for (int i = 0; i < 5; i++) drawTruck(i);
    for (int i = 0; i < 5; i++) {
        if (truckContainerPos[i][0] != -100) {
            int colorIndex = (truckContainerColor[i] >= 0 && truckContainerColor[i] < 12) ? truckContainerColor[i] : 0;
            drawContainer(truckContainerPos[i][0], truckContainerPos[i][1], truckContainerPos[i][2],
                          containerColors[colorIndex], -1);
        }
    }
    for (int i = 0; i < 2; i++) drawCar(i);
    for (int i = 0; i < 122; i++) {
        if (!containerGrabbed || currentContainer != i) {
            drawContainer(containerPos[i][0], containerPos[i][1], containerPos[i][2], containerColors[i % 12], i);
        }
    }
    for (int i = 0; i < 8; i++) {
        drawWorker(workerX[i], 0.0f, workerZ[i]);
    }
    drawWorker(-10.0f, 0.5f, 9.0f);
    drawWorker(0.0f, 0.5f, 9.0f);
    drawWorker(10.0f, 0.5f, 9.0f);
    drawWorker(0.0f, 0.5f, 11.0f);
    drawWarehouse();
    drawSeaBank();
    drawRoads();
    drawCone(0.0f, 0.5f, 10.0f);
    drawCone(0.0f, 0.5f, 11.0f);
    drawTree(-12.0f, 8.5f);
    drawTree(-6.0f, 8.5f);
    drawTree(0.0f, 8.5f);
    drawTree(6.0f, 8.5f);
    drawTree(12.0f, 8.5f);
    drawTree(0.0f, 9.5f);
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
        float camX = cameraDistance * sin(cameraAngleX) * cos(cameraAngleY);
        float camY = cameraDistance * sin(cameraAngleY);
        float camZ = cameraDistance * cos(cameraAngleX) * cos(cameraAngleY);
        float dirX = posX - camX, dirY = posY - camY, dirZ = posZ - camZ;
        float len = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
        dirX /= len; dirY /= len; dirZ /= len;
        selectedShip = -1;
        float tMin = 1e10;
        for (int i = 0; i < 2; i++) {
            float sX = (i == 0) ? shipX : ship2X;
            float sZ = (i == 0) ? shipZ : ship2Z;
            float minX = sX - 4.0f, maxX = sX + 4.0f;
            float minY = 0.0f, maxY = 3.0f;
            float minZ = sZ - 4.0f, maxZ = sZ + 4.0f;
            float t1 = (minX - camX) / dirX, t2 = (maxX - camX) / dirX;
            float t3 = (minY - camY) / dirY, t4 = (maxY - camY) / dirY;
            float t5 = (minZ - camZ) / dirZ, t6 = (maxZ - camZ) / dirZ;
            float tNear = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
            float tFar = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
            if (tNear < tFar && tNear > 0 && tNear < tMin) {
                tMin = tNear;
                selectedShip = i;
            }
        }
        selectedTruck = -1;
        tMin = 1e10;
        for (int i = 0; i < 5; i++) {
            float tX = truckX[i], tZ = truckZ[i];
            float minX = tX - 2.0f, maxX = tX + 2.0f;
            float minY = truckY - 0.5f, maxY = truckY + 1.0f;
            float minZ = tZ - 1.0f, maxZ = tZ + 1.0f;
            float t1 = (minX - camX) / dirX, t2 = (maxX - camX) / dirX;
            float t3 = (minY - camY) / dirY, t4 = (maxY - camY) / dirY;
            float t5 = (minZ - camZ) / dirZ, t6 = (maxZ - camZ) / dirZ;
            float tNear = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
            float tFar = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
            if (tNear < tFar && tNear > 0 && tNear < tMin) {
                tMin = tNear;
                selectedTruck = i;
            }
        }
        if (selectedTruck != -1) {
            std::cout << "Selected truck: " << selectedTruck << std::endl;
        }
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            if (selectedShip == 0 && shipZ + 0.2f >= -13.0f) {
                ship1MovingForward = false;
                break;
            }
            if (selectedShip == 1 && ship2Z + 0.2f >= -13.0f) {
                ship2MovingForward = false;
                break;
            }
            if (selectedShip == 0) {
                shipZ += 0.2f;
                ship1MovingForward = true;
            }
            if (selectedShip == 1) {
                ship2Z += 0.2f;
                ship2MovingForward = true;
            }
            break;
        case 'z':
            if (selectedShip == 0) {
                shipZ -= 0.2f;
                ship1MovingForward = false;
            }
            if (selectedShip == 1) {
                ship2Z -= 0.2f;
                ship2MovingForward = false;
            }
            break;
        case 'a':
            if (selectedShip == 0) shipX -= 0.2f;
            if (selectedShip == 1) ship2X -= 0.2f;
            break;
        case 's':
            if (selectedShip == 0) shipX += 0.2f;
            if (selectedShip == 1) ship2X += 0.2f;
            break;

      //truck control
       case 'i':
            if (selectedTruck != -1) {
                truckX[selectedTruck] += 0.2f;
                if (selectedTruck == 3 && truckContainerPos[3][0] != -100) {
                    truckContainerPos[3][0] += 0.2f;
                    std::cout << "Truck 3 container moved to: (" << truckContainerPos[3][0] << ", "
                              << truckContainerPos[3][1] << ", " << truckContainerPos[3][2] << ")" << std::endl;
                }
            }
            break;
        case 'm':
            if (selectedTruck != -1) {
                truckX[selectedTruck] -= 0.2f;
                if (selectedTruck == 3 && truckContainerPos[3][0] != -100) {
                    truckContainerPos[3][0] -= 0.2f;
                    std::cout << "Truck 3 container moved to: (" << truckContainerPos[3][0] << ", "
                              << truckContainerPos[3][1] << ", " << truckContainerPos[3][2] << ")" << std::endl;
                }
            }
            break;
        case 'j':
            if (selectedTruck != -1) {
                truckZ[selectedTruck] -= 0.2f;
                if (selectedTruck == 3 && truckContainerPos[3][0] != -100) {
                    truckContainerPos[3][2] -= 0.2f;
                    std::cout << "Truck 3 container moved to: (" << truckContainerPos[3][0] << ", "
                              << truckContainerPos[3][1] << ", " << truckContainerPos[3][2] << ")" << std::endl;
                }
            }
            break;
        case 'k':
            if (selectedTruck != -1) {
                truckZ[selectedTruck] += 0.2f;
                if (selectedTruck == 3 && truckContainerPos[3][0] != -100) {
                    truckContainerPos[3][2] += 0.2f;
                    std::cout << "Truck 3 container moved to: (" << truckContainerPos[3][0] << ", "
                              << truckContainerPos[3][1] << ", " << truckContainerPos[3][2] << ")" << std::endl;
                }
            }
            break;

         //cars control
        case '1': carZ[0] -= 0.2f; break;
        case '2': carZ[0] += 0.2f; break;
        case '3': carX[0] -= 0.2f; break;
        case '4': carX[0] += 0.2f; break;
        case '5': carZ[1] -= 0.2f; break;
        case '6': carZ[1] += 0.2f; break;
        case '7': carX[1] -= 0.2f; break;
        case '8': carX[1] += 0.2f; break;
        case 'g':
            if (!craneWorking && isShip1Anchored) {
                craneWorking = true;
                currentContainer = (currentContainer + 1) % 12;
                containerGrabbed = false;
                std::cout << "Crane started, grabbing container: " << currentContainer << std::endl;
            }
            break;
        case 'h':
            if (containerGrabbed && !craneWorking /*&& selectedTruck == 3*/) { // Relaxed condition for testing
                std::cout << "Releasing container on truck 3" << std::endl;
                truckContainerPos[3][0] = truckX[3]+1.4f; // Adjusted for truck rotation
                truckContainerPos[3][1] = truckY+1.5f;    // Above truck bed
                truckContainerPos[3][2] = truckZ[3];
                truckContainerColor[3] = currentContainer % 12;
                std::cout << "Container placed at: (" << truckContainerPos[3][0] << ", "
                          << truckContainerPos[3][1] << ", " << truckContainerPos[3][2]
                          << "), color: " << truckContainerColor[3] << std::endl;
                containerGrabbed = false;
                currentContainer = -1; // Reset
                truckX[3] += 0.0f;


                if (truckX[3] > 0.0f)
                    truckX[3] = -1.0f;
                 }

            else {
                std::cout << "Cannot release: containerGrabbed=" << containerGrabbed
                          << ", craneWorking=" << craneWorking
                          << ", selectedTruck=" << selectedTruck
                          << ", craneAngle=" << craneAngle
                          << ", hookHeight=" << hookHeight << std::endl;
            }
            break;
        case 'd': // Debug key
            std::cout << "Debug: containerGrabbed=" << containerGrabbed
                      << ", craneWorking=" << craneWorking
                      << ", selectedTruck=" << selectedTruck
                      << ", craneAngle=" << craneAngle
                      << ", hookHeight=" << hookHeight
                      << ", currentContainer=" << currentContainer << std::endl;
            if (truckContainerPos[3][0] != -100) {
                std::cout << "Truck 3 container at: (" << truckContainerPos[3][0] << ", "
                          << truckContainerPos[3][1] << ", " << truckContainerPos[3][2]
                          << "), color: " << truckContainerColor[3] << std::endl;
            }
            break;
         case 'o':
            // Toggle target door angle for smooth animation
            targetDoorAngle = (targetDoorAngle == 0.0f) ? 90.0f : 0.0f;
            break;


            // Zoom in and out
        case '+': // Zoom in
            cameraDistance -= 1.0f;
            if (cameraDistance < minCameraDistance) {
                cameraDistance = minCameraDistance;
            }
            std::cout << "Zoom in, cameraDistance: " << cameraDistance << std::endl;
            break;
        case '-': // Zoom out
            cameraDistance += 1.0f;
            if (cameraDistance > maxCameraDistance) {
                cameraDistance = maxCameraDistance;
            }
            std::cout << "Zoom out, cameraDistance: " << cameraDistance << std::endl;
            break;

        case 27: exit(0);
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: cameraAngleX -= 0.1f; break;
        case GLUT_KEY_RIGHT: cameraAngleX += 0.1f; break;
        case GLUT_KEY_UP: cameraAngleY += 0.1f; break;
        case GLUT_KEY_DOWN: cameraAngleY -= 0.1f; break;
    }
    glutPostRedisplay();
}

void update(int value) {
    waveTime += 0.05f;

    // Smoke emission timer
    smokeEmissionTimer += 0.05f;
    if (smokeEmissionTimer >= smokeEmissionRate) {
        smokeEmissionTimer = 0.05f; // Reset timer

        // Emit a new particle for Ship 1 if it's not anchored
        if (!isShip1Anchored) {
            for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
                if (!smokeParticlesShip1[i].active) {


                    smokeParticlesShip1[i].x = 4.5f;   // x should be same as chimneys
                    smokeParticlesShip1[i].y = 5.3f;   // y should be slightly increased of actual y , so that the smoke can be released just above the chimneys
                    smokeParticlesShip1[i].z = 0.0f;// z should be same as chimneys

                    smokeParticlesShip1[i].vx = (rand() % 100 - 50) / 500.0f; // Random drift
                    smokeParticlesShip1[i].vz = (rand() % 100 - 50) / 500.0f;
                    smokeParticlesShip1[i].lifetime = 3.0f;   // Particle lasts 3 seconds
                    smokeParticlesShip1[i].active = true;
                    break;
                }
            }
        }

        // Emit a new particle for Ship 2 if it's not anchored
        if (!isShip2Anchored) {
            for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
                if (!smokeParticlesShip2[i].active) {
                    smokeParticlesShip1[i].x = 4.5f;   // x should be same as chimneys
                    smokeParticlesShip1[i].y = 5.3f;   // y should be slightly increased of actual y , so that the smoke can be released just above the chimneys
                    smokeParticlesShip1[i].z = 0.0f;// z should be same as chimneys
                    smokeParticlesShip2[i].vx = (rand() % 100 - 50) / 500.0f;
                    smokeParticlesShip2[i].vz = (rand() % 100 - 50) / 500.0f;
                    smokeParticlesShip2[i].lifetime = 3.0f;
                    smokeParticlesShip2[i].active = true;
                    break;
                }
            }
        }
    }

    // Update smoke particles for Ship 1
    for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
        if (smokeParticlesShip1[i].active) {
            smokeParticlesShip1[i].y += 0.1f; // Move upward
            smokeParticlesShip1[i].x += smokeParticlesShip1[i].vx; // Apply drift
            smokeParticlesShip1[i].z += smokeParticlesShip1[i].vz;
            smokeParticlesShip1[i].lifetime -= 0.05f;
            if (smokeParticlesShip1[i].lifetime <= 0.0f) {
                smokeParticlesShip1[i].active = false; // Deactivate particle
            }
        }
    }

    // Update smoke particles for Ship 2
    for (int i = 0; i < NUM_SMOKE_PARTICLES; i++) {
        if (smokeParticlesShip2[i].active) {
            smokeParticlesShip2[i].y += 0.1f;
            smokeParticlesShip2[i].x += smokeParticlesShip2[i].vx;
            smokeParticlesShip2[i].z += smokeParticlesShip2[i].vz;
            smokeParticlesShip2[i].lifetime -= 0.05f;
            if (smokeParticlesShip2[i].lifetime <= 0.0f) {
                smokeParticlesShip2[i].active = false;
            }
        }
    }


    if (craneWorking) {
        static bool movingToShip = true;
        static bool lowering = false;
        static bool lifting = false;
        if (movingToShip) {
            if (craneAngle > -330.0f) {
                craneAngle -= 1.0f;
            } else {
                movingToShip = false;
                lowering = true;
            }
        } else if (lowering) {
            if (hookHeight < 2.0f) {
                hookHeight += 0.15f;
            } else {
                lowering = false;
                containerGrabbed = true;
                std::cout << "Container grabbed, currentContainer: " << currentContainer << std::endl;
                lifting = true;
            }
        } else if (lifting) {
            if (hookHeight > 1.0f) {
                hookHeight -= 0.15f;
            } else {
                if (craneAngle < 0.0f) {
                    craneAngle += 1.0f;
                } else {
                    craneWorking = false;
                    lifting = false;
                    movingToShip = true;
                    std::cout << "Crane cycle complete, ready to release" << std::endl;
                }
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        workerX[i] += workerSpeedX[i];
        workerZ[i] += workerSpeedZ[i];
        if (workerX[i] < -15.0f || workerX[i] > 15.0f) workerSpeedX[i] *= -1;
        if (workerZ[i] < -6.0f || workerZ[i] > 6.0f) workerSpeedZ[i] *= -1;
    }
    isShip1Anchored = (fabs(shipZ - (-13.0f)) < 0.5f && fabs(shipX) < 5.0f);
    isShip2Anchored = (fabs(ship2Z - (-13.0f)) < 0.5f && fabs(ship2X) < 5.0f);

        // Manage ship sounds
// Replace ship sound lines with:
    if (!isShip1Anchored && !ship1SoundPlaying) {
            PlaySound(TEXT("waves.wav"), NULL, SND_FILENAME | SND_ASYNC);
    PlaySound(TEXT("ship_engine.wav"), NULL, SND_FILENAME | SND_ASYNC);
     //sndPlaySound(TEXT("ship_engine.wav"), SND_ASYNC);
    ship1SoundPlaying = true;
}
   if (!isShip2Anchored && !ship2SoundPlaying) {
        PlaySound(TEXT("waves.wav"), NULL, SND_FILENAME | SND_ASYNC);
        PlaySound(TEXT("ship_engine.wav"), NULL, SND_FILENAME | SND_ASYNC);
    //sndPlaySound(TEXT("ship_engine.wav"), SND_ASYNC);
    ship2SoundPlaying = true;
}

    // Manage crane sound
    if (craneWorking && !craneSoundPlaying) {
            PlaySound(TEXT("waves.wav"), NULL, SND_FILENAME | SND_ASYNC);
       //sndPlaySound(TEXT("crane_movement.wav"), SND_ASYNC);
       PlaySound(TEXT("crane_movement.wav"), NULL, SND_FILENAME | SND_ASYNC);
        craneSoundPlaying = true;
    }
    else if (!craneWorking && craneSoundPlaying) {
        PlaySound(NULL, NULL, SND_PURGE); // Stop crane sound
        craneSoundPlaying = false;
    }
    // Updated door animation code
    // Animate door rotation
    float doorSpeed = 90.0f; // 90 degrees per second
    float deltaTime = 0.05f; // 50ms update interval
    if (fabs(doorAngle - targetDoorAngle) > 0.1f) {
        // Play sound when door starts moving
        if (!doorSoundPlayed) {
                PlaySound(TEXT("doorsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
            //sndPlaySound(TEXT("doorsound.wav"), SND_ASYNC);
            doorSoundPlayed = true;
        }

        if (doorAngle < targetDoorAngle) {
            doorAngle = std::min(doorAngle + doorSpeed * deltaTime, targetDoorAngle);
        } else {
            doorAngle = std::max(doorAngle - doorSpeed * deltaTime, targetDoorAngle);
        }
        // Debug output to verify gateOpen and doorAngle
        std::cout << "Door angle: " << doorAngle << ", Target: " << targetDoorAngle << ", gateOpen: " << gateOpen << std::endl;
    } else {
        // Reset sound flag when door stops moving
        doorSoundPlayed = false;
    }
    // Update gateOpen based on door angle
    gateOpen = (doorAngle > 45.0f); // Show white interior when door is more than halfway open

    glutPostRedisplay();
    glutTimerFunc(50, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 500);
    glutCreateWindow("Port Scenario with Updated Operations");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutTimerFunc(50, update, 0);
    glutMainLoop();
    return 0;
}
