#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <sstream>

//Vars
int windowWidth = 800;
int windowHeight = 600;

struct Missile {
    float x;
    float y;
};

std::vector<Missile> missiles;

struct Sub {
    float x;
    float y;
};

std::vector<Sub> enemies;

int enemies_number = 0;
int enemies_limit = 5;
int enemies_speed = 1000;

Sub boat = {-0.8f, 0.0f};
bool imortal = false;
bool visible = true;
bool blink = false;
bool dead = false;

bool air_drop = true;
float air = 5.00f;
int life = 2;

int points = 0;

void sub() {

    glLoadIdentity();
    
    if (visible) {
        glTranslatef(boat.x + 0.15, boat.y + 0.06, 0.0f);

        glColor3f(0.5f, 0.5f, 0.5f);

        glRectf(-0.15f, -0.05f, 0.15f, 0.05f);
        glRectf(0.0f, 0.00f, 0.07f, 0.09f);
    }
}

void missile() {
    // Desenhar os mísseis
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const auto& missile : missiles) {
        glLoadIdentity();
        glTranslatef(missile.x, missile.y, 0.0f);

        glRectf(0.01f, 0.01f,-0.03f, -0.01f);
    }
}

void subEnemy() {
    glLoadIdentity();

    glColor3f(1.0f, 0.0f, 0.0f);  // Vermelho
    for (const auto& enemy : enemies) {
        glRectf(enemy.x , enemy.y, enemy.x + 0.1f, enemy.y + 0.1f);
    }
}

void air_bar() {
    glLoadIdentity();
    glTranslatef(-0.9f, 0.9f, 0.0f);

    // Desenhar o fundo da barra de vida (cor cinza)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 0.05f);
        glVertex2f(1.5f, 0.05f);
        glVertex2f(1.5f, 0.0f);
    glEnd();

    // Desenhar a barra de vida atual (cor verde)
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 0.05f);
        glVertex2f(0.3f * air, 0.05f);
        glVertex2f(0.3f * air, 0.0f);
    glEnd();

    glPopMatrix();
}

void lifes() {
    glLoadIdentity();
    glTranslatef(-0.9f, 0.8f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    for (int i = 0; i < life; i++) {
        glRectf(0.05f, 0.05f, 0.0f, 0.0f);
        glTranslatef(0.1f, 0.0f, 0.0f);
    }

    glFlush();
}

void counter() {
    // Converter o valor do contador para uma string
    std::stringstream ss;
    ss << points;
    std::string strNumber = ss.str();

    // Posição inicial do contador
    float posX = 0.7f;
    float posY = 0.9f;

    // Definir a cor do texto
    glColor3f(1.0f, 1.0f, 1.0f);

    // Exibir cada caractere da string na posição adequada
    for (char i : strNumber) {
        glRasterPos2f(posX, posY);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, i);
        posX += 0.06f;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    sub();
    missile();
    air_bar();
    lifes();
    subEnemy();
    counter();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            if (boat.y + 0.1f < 0.8f)
                boat.y += 0.1f;
            break;
        case 'a':
            if (boat.x - 0.1f > -1.0f)
                boat.x -= 0.1f;
            break;
        case 's':
            if (boat.y - 0.1f > -1.0f)
                boat.y -= 0.1f;
            break;
        case 'd':
            if (boat.x + 0.1f < 1.0f)
                boat.x += 0.1f;
            break;
        case ' ':
            missiles.push_back({ boat.x + 0.17f, boat.y + 0.06f });
            break;
        case 'q':
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void revive(int value) {
    blink = false;
    visible = true;
    imortal = false;
}

void updateSub(int value) {
    if (dead) {
        return;
    }

    for (auto& enemy : enemies) {
        enemy.x -= 0.025f;
        if (enemy.x < -1.0f) {
            enemy = enemies.back();
            enemies.pop_back();
            enemies_number--;
        }
    }

    if (enemies_number <= enemies_limit) {
        enemies_number++;
        float randomY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (0.7f - (-0.7f)) + (-0.7f);
        enemies.push_back({ 1.0f , randomY });
    }

    glutPostRedisplay();
    glutTimerFunc(enemies_speed, updateSub, 0);
}

void updateColission(int value) {
    //Colissäo entre Missil e Sub Inimigo
    for (auto& missile : missiles) {
        for (auto& enemy : enemies) { 

            if (enemy.y + 0.1f > missile.y && enemy.y < missile.y) {
                if (enemy.x + 0.1f > missile.x && enemy.x < missile.x) {

                    missile = missiles.back();
                    missiles.pop_back();

                    enemy = enemies.back();
                    enemies.pop_back();
                    enemies_number --;

                    if (enemies_speed - 5 >= 1) {
                        enemies_speed -= 5;
                    }

                    points++;

                    if (points % 20 == 0) {
                        life++;
                        enemies_limit ++;
                    }
                }
            }

        }
    }

    //Colissao entre Sub e Sub Inimigo
    if (!imortal) {
        for (auto& enemy : enemies) {
            if (boat.x < enemy.x + 0.1f && boat.y + 0.14f > enemy.y) {
                if (boat.y < enemy.y + 0.1f && boat.x + 0.3f > enemy.x) {
                    if (life == 0) {
                        dead = true;
                        exit(0);
                        break;
                    }
                    life--;
                    boat.x = -0.8f;
                    boat.y = 0.0f;
                    imortal = true;
                    blink = true;

                    glutTimerFunc(2000, revive, 0);
                }
            }
        }
    }

    glutTimerFunc(16, updateColission, 0);
}

void updateMissele(int value) {
    if (dead) {
        return;
    }

    for (auto& missile : missiles) {
        missile.x += 0.03f;

        if (missile.x > 1.5f) {
            missile = missiles.back();
            missiles.pop_back();
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, updateMissele, 0);
}

void updateAir(int value) {
    if (dead) {
        return;
    }

    if (air_drop) {
        air -= 0.1f;

        if (air <= 0.0f) {
            if (life > 0) {
                life--;
                air = 5.0f;
            }
            else {
                //Game over
                dead = true;
                air_drop = false;
                air = 0.0f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(1000, updateAir, 0);
}

void updateBlink(int value) {
    if (dead) {
        return;
    }

    if (blink) {
        visible = !visible;
    }

    glutPostRedisplay();
    glutTimerFunc(150, updateBlink, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Seaquest");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    //timers
    glutTimerFunc(0, updateMissele, 0);
    glutTimerFunc(0, updateAir, 0);
    glutTimerFunc(0, updateSub, 0);
    glutTimerFunc(0, updateColission, 0);
    glutTimerFunc(0, updateBlink, 0);

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    glutMainLoop();

    return 0;
}