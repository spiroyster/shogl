#define SHOGL_OPENGL_10             // Force GL1.0 context.
#include "../include/shogl.hpp"     // Include shogl...

float angle = 0.0f;

SHOGL()
{
    shogl()->window_title("Hello triangle (callback) GL1");
    shogl()->window_fps(60);

    shogl()->draw([=]() 
        {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotatef(angle, 0, 0, 1.0f);

            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0, 0);
            glVertex3f(-0.5f, -0.5f, -1.0f);
            glColor3f(0, 1.0f, 0);
            glVertex3f(0, 0.5f, -1.0f);
            glColor3f(0, 0, 1.0f);
            glVertex3f(0.5f, -0.5f, -1.0f);
            glEnd();
        });

    shogl()->idle([=]()
        {
            angle += 0.1f;
        });

    shogl()->resize([](int width, int height) 
        {
            glViewport(0, 0, width, height);
        });

    shogl()->key_down([](int x, int y, unsigned int key)
        {
            shogl()->window_quit(0);
        });

}