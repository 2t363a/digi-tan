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

static constexpr const double kAspect = 3840.0 / 2160.0;

struct State {
    struct Point { double x; double y; };

    struct Snowball {
        double x;
        double y;
        double r;
        double xdot;
        double ydot;
        double r3 = 0.0;

        bool exists = true;
    };

    static constexpr const unsigned kM = 600;
    static constexpr const unsigned kN = 192;

    long n_mergers = 0;

    std::chrono::steady_clock::time_point t0;
    std::chrono::steady_clock::time_point t1;

    std::array<Point, kN> circle;
    std::vector<Snowball> ball_storage[2];
    int active = 0;
    std::vector<Snowball>* snowballs[2];

    State() {
        t0 = std::chrono::steady_clock::now();
        t1 = std::chrono::steady_clock::now();

        snowballs[0] = &ball_storage[0];
        snowballs[1] = &ball_storage[1];
        active = 0;

#if 0
    double r1 = 0.5;
    snowballs[active]->push_back(Snowball{.x = 0.3, .y = 0, .r = r1, .xdot = 0.067 * 1, .ydot = 0, .r3 = r1 * r1 * r1});
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
        auto& balls = *snowballs[active];
        /// wall collision
        for (auto& ball: balls) {
            if (not ball.exists) { continue; }
            double xdot = ball.xdot;
            double ydot = ball.ydot;
            double x = ball.x + dt * xdot;
            double y = ball.y + dt * ydot;
            double r = ball.r;

            do {
                /// --- Правая сторона
                if (x + r > 1.0) {
                    double nx = 2.0 - x - r * 2;
                    x = nx;
                    xdot = -xdot;
                }
                /// --- Левая
                else if (x - r < -1.0) {
                    double nx = -2.0 - x + r * 2;
                    x = nx;
                    xdot = -xdot;
                }
                if (y + r > 1.0) {
                    y = 2 - y - r * 2;
                    ydot = -ydot;
                }
                else if (y - r < -1.0) {
                    y = -2.0 - y + r * 2;
                    ydot = -ydot;
                }
                ball.x = x;
                ball.y = y;
                ball.xdot = xdot;
                ball.ydot = ydot;
            } while (0);
        }

        /// Плоское притяжение -- простой N^2
        for (unsigned i = 0; i < balls.size(); ++i) {
            double ex = 0.0;
            double ey = 0.0;
            if (not balls[i].exists) { continue; }
            for (unsigned j = 0; j < balls.size(); ++j) {
                if (not balls[j].exists) { continue; }
                if (i == j) { continue; }
                double dx = balls[i].x - balls[j].x;
                double dy = balls[i].y - balls[j].y;
                double r = dx * dx + dy * dy;
                ex += dx * balls[j].r / r;
                ey += dy * balls[j].r / r;
            }
        }
    }

    void physics() {
        snowballs[!active]->clear();
        auto &local_snowballs = *snowballs[active];
        bool coalesce = false;

        for (unsigned i = 0; i < local_snowballs.size(); ++i) {
            auto ball = local_snowballs[i];
            if (not ball.exists) { continue; }

            double xi = ball.x;
            double yi = ball.y;
            double ri = ball.r;

            for (unsigned j = 0; j < local_snowballs.size(); ++j) {
                if (j == i) { continue; }
                auto ball2 = local_snowballs[j];
                if (not ball2.exists) { continue; }

                double xj = ball2.x;
                double yj = ball2.y;
                double rj = ball2.r;

                double rij = ri + rj;

                double dx = xj - xi;
                double dy = yj - yi;
                if (fabs(dx) > rij) { continue; }
                if (fabs(dy) > rij) { continue; }

                double dx2 = dx * dx;
                double dy2 = dy * dy;

                if (sqrt(dx2 + dy2) < rij) {
                    coalesce = true;
                    double x = (ball.x * ri + ball2.x * rj) / (rij);
                    double y = (ball.y * ri + ball2.y * rj) / (rij);
                    double xdot = (ball.xdot * ri + ball2.xdot * rj) / (rij);
                    double ydot = (ball.ydot * ri + ball2.ydot * rj) / (rij);

                    unsigned k = (ri >= rj) ? i : j;
                    unsigned kk = (ri < rj) ? i : j;
                    local_snowballs[k].exists = false;
                    local_snowballs[kk].exists = false;

                    n_mergers += 1;
                    ball.x = x;
                    ball.y = y;
                    ball.xdot = xdot;
                    ball.ydot = ydot;
                    double eff = 0.95;
                    while (rij * eff < ri or rij * eff < rj) { eff = 1 - (1 - eff) / 2; }
                    fprintf(stderr, "--- merger %ld\n", n_mergers);
                    fprintf(stderr, "%lf and %lf merge with eff. %lf to form %lf\n", ri, rj, eff, eff * rij);
                    fprintf(stderr, "--- merger\n");
                    ball.r = eff * rij; /// energy loss
                    ball.exists = true;
                    if (ball.r > 1.0) { std::quick_exit(0); }
                    break;
                }
            } // for (j = 0; ... )
            snowballs[!active]->push_back(ball);
        } // for (i = 0; ...)
        active = !active;
    }

    void draw() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE /* GL_FILL */);

        for (auto const& ball : *snowballs[active]) {
            if (not ball.exists) { continue; }

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glTranslatef(ball.x, ball.y, 0.0);
            glScalef(ball.r, ball.r, 1.0);

            glBegin(GL_POLYGON);
            glColor3d(1.0, 1.0, 1.0);

            for (int i = 0; i < circle.size(); ++i) {
                glVertex2d(circle[i].x, circle[i].y);
            }

            glEnd();

            glBegin(GL_POLYGON);
            glColor3d(1.0, 1.0, 1.0);

            for (int i = 0; i < circle.size(); ++i) {
                glVertex2d(circle[i].x * 0.01, circle[i].y * 0.01);
            }
            glEnd();
        }
    }

    void add_snowball(Snowball b) {
        snowballs[active]->push_back(b);
    }
}; /// struct State

static State g_state;

static void
init(Random& rng) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double kWorldX = 1.75;
    double kWorldY = 1.5; 

    glOrtho(-kAspect, kAspect, kWorldY, -kWorldY, -1.0, 1.0);

    double sum_r3 = 0;
    for (int i = 0; i < State::kM; ++i) {
        double x = 1.0 - rng() * 0x1p-63;
        double y = 1.0 - rng() * 0x1p-63;
        double r = 0.01;
        double xdot = (1.0 - rng() * 0x1p-64) * 0.25;
        double ydot = (1.0 - rng() * 0x1p-64) * 0.25;

        sum_r3 += r * r * r;
        g_state.add_snowball(State::Snowball{.x = x, .y = y, .r = r, .xdot = xdot, .ydot = ydot, .r3 = r * r * r});
    } /// for (int i = 0; ...)
} /// init()

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

    /// draw state-dependent
    g_state.draw();
    // glFlush();
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
    g_state.move(dt * 0x1p-30);
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

