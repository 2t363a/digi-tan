#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <random>
#include <thread>
#include <vector>

#include <GL/glut.h>   // Header path for Windows and Linux

using Random = std::mt19937_64;

struct State {
    struct Point { double x; double y; };

    struct Ball {
        double x;
        double y;
        double r;
        double xdot;
        double ydot;
        double r3 = 0.0;

        bool exists = true;
    };

    static constexpr const unsigned kM = 1'000;
    static constexpr const unsigned kN = 96;

    std::chrono::steady_clock::time_point t0;
    std::chrono::steady_clock::time_point t1;

    std::array<Point, kN> circle;
    std::vector<Ball> balls1;
    std::vector<Ball> balls2;
    std::vector<Ball>* balls;
    std::vector<Ball>* balls_next;


    State() {
        t0 = std::chrono::steady_clock::now();
        t1 = std::chrono::steady_clock::now();

        balls = &balls1;
        balls_next = &balls2;

#if 0
        balls->push_back(Ball{.x = -0.5, .y = -0.5, .r = 0.02, .xdot = 0.067 * 0x1p-30 * 5, .ydot = 0.069 * 0x1p-30 * 5, .r3 = 0.02 * 0.02 * 0.02});
        balls->push_back(Ball{.x = 0.5, .y = 0.5, .r = 0.02, .xdot = -0.067 * 0x1p-30 * 5, .ydot = -0.069 * 0x1p-30 * 5, .r3 = 0.02 * 0.02 * 0.02});
#endif
        double x = 1.0;
        double y = 0.0;
        double dc = cos(2 * M_PI / kN);
        double ds = sin(2 * M_PI / kN);

        for (unsigned i = 0; i < kN; ++i) {
            circle[i].x = x;
            circle[i].y = y;

            double t1 = x * dc - y * ds;
            double t2 = y * dc + x * ds;

            x = t1;
            y = t2;
        }
    }

    void move(double dt) {
        for (auto& ball: *balls) {
            double xdot = ball.xdot;
            double ydot = ball.ydot;
            double x = ball.x + dt * xdot;
            double y = ball.y + dt * ydot;
            double r = ball.r;

            do {
                /// --- Правая сторона
                if (x + r > 1.0) {
                    x = 2 - (x + r) - r;
                    xdot = -xdot;
                }
                else if (x - r < -1.0) {
                    x = -2.0 - (x - r) + r;
                    xdot = -xdot;
                }
                if (y + r > 1.0) {
                    y = 2 - (y + r) - r;
                    ydot = -ydot;
                }
                else if (y - r < -1.0) {
                    y = -2.0 - (y - r) + r;
                    ydot = -ydot;
                }
                ball.x = x;
                ball.y = y;
                ball.xdot = xdot;
                ball.ydot = ydot;
            } while (0);
        }
    }

    void physics() {
        balls_next->clear();
        auto &local_balls = *balls;
        bool coalesce = true;

        for (unsigned i = 0; i < local_balls.size(); ++i) {
            coalesce = false;
            auto ball = local_balls[i];
            if (not ball.exists) { continue; }

            double x0 = ball.x;
            double y0 = ball.y;
            double r0 = ball.r;
            double r30 = ball.r3;

            for (unsigned j = 0; j < local_balls.size(); ++j) {
                if (j == i) { continue; }
                auto ball2 = local_balls[j];
                if (not ball2.exists) { continue; }

                double x1 = ball2.x;
                double y1 = ball2.y;
                double r1 = ball2.r;
                double r31 = ball2.r3;

                double dx = x1 - x0;
                double dy = y1 - y0;
                if (fabs(dx) + fabs(dy) > r0 + r1) { continue; }

                double dx2 = dx * dx;
                double dy2 = dy * dy;
                if (sqrt(dx2 + dy2) < r0 + r1) {
                    coalesce = true;
                    double xdot = (ball.xdot * r30 + ball2.xdot * r31) / (r30 + r31);
                    double ydot = (ball.ydot * r30 + ball2.ydot * r31) / (r30 + r31);

                    unsigned k = (r0 < r1) ? i : j;
                    local_balls[k].exists = false;

                    ball.exists = true;
                    ball.xdot = xdot;
                    ball.ydot = ydot;
                    ball.r = cbrt(r30 + r31);
                    ball.r3 = r30 + r31;
                    if (ball.r > 0.5) { ball.r = 0.5; }
                    break;
                }
            } // for (j = 0; ... )
            balls_next->push_back(ball);
        } // for (i = 0; ...)
        std::swap(balls, balls_next);
    }
}; /// struct State

static State g_state;

static void
init(Random& rng) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double kWorld = 3;
    glOrtho(-kWorld, kWorld, kWorld, -kWorld, -1.0, 1.0);

    for (int i = 0; i < State::kM; ++i) {
        double x = 1.0 - rng() * 0x1p-63;
        double y = 1.0 - rng() * 0x1p-63;
        double r = 0.01;
        double xdot = (1.0 - rng() * 0x1p-64) * 0.25 * 0x1p-30;
        double ydot = (1.0 - rng() * 0x1p-64) * 0.25 * 0x1p-30;

        g_state.balls->push_back(State::Ball{.x = x, .y = y, .r = r, .xdot = xdot, .ydot = ydot, .r3 = r * r * r});
    }
}

static void
draw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_POLYGON);
        glColor3d(1.0, 1.0, 0.0);
        glVertex2d(1.0, -1.0);
        glVertex2d(1.0, +1.0);
        glVertex2d(-1.0, +1.0);
        glVertex2d(-1.0, -1.0);
    glEnd();

    glBegin(GL_QUADS);
        glColor3d(1.0, 0.0, 1.0);
        glVertex2d(1.0, -1.0);
        glVertex2d(1.0, +1.0);
        glVertex2d(-1.0, +1.0);
        glVertex2d(-1.0, -1.0);

        glVertex2d(0.0, -1.0);
        glVertex2d(1.0, -1.0);
        glVertex2d(1.0, +1.0);
        glVertex2d(0, +1.0);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE /* GL_FILL */);

    for (auto const& ball : *g_state.balls) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(ball.x, ball.y, 0.0);
        glScalef(ball.r * 9.0 / 16.0, ball.r, 1.0);

        glBegin(GL_POLYGON);
        glColor3d(1.0, 1.0, 1.0);

        for (int i = 0; i < g_state.circle.size(); ++i) {
            glVertex2d(g_state.circle[i].x, g_state.circle[i].y);
        }

        glEnd();
    }
    glFlush();

    glutSwapBuffers();
}

static void
keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: glutLeaveGameMode(); exit(0); break;
    }
}

static void
idle() {
    using namespace std::chrono_literals; // Enables suffix literals like 's' or 'ms'
    auto t2 = std::chrono::steady_clock::now();
    auto dt = (t2 - g_state.t1).count();
    if (dt < 30'000'000) {
        dt = 30'000'000 - dt;
        std::this_thread::sleep_for(dt * 1ns);
    }
    g_state.t1 = t2;
    g_state.move(dt);
    g_state.physics();
    glutPostRedisplay();
}


int main(int argc, char** argv) {
    std::random_device rd;
    Random rng(rd() ^ rd());

    // Initialize GLUT and process command-line arguments
    glutInit(&argc, argv);

    glutGameModeString("3840x2160");
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
        glutEnterGameMode();
    } else {
        // Fallback to standard window if the resolution isn't supported
        glutInitWindowSize(3000, 2000);
        glutInitWindowPosition(0, 0);
        glutCreateWindow("Fallback Window");
    }


    init(rng);

    // Register the display callback function
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard); // Crucial to prevent getting locked out of the desktop
    glutIdleFunc(idle);
    // Enter the infinite event-processing loop
    glutMainLoop();
    return 0;
}

