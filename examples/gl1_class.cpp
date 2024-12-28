#define SHOGL_OPENGL_10             // Force GL1.0 context.
#include "../include/shogl.hpp"     // Include shogl...

class myWindow : public shogl_window
{
    float angle_ = 0.0f;

public:

    void draw()
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle_ += 0.1f, 0, 0, 1.0f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0, 0);
        glVertex3f(-0.5f, -0.5f, -1.0f);
        glColor3f(0, 1.0f, 0);
        glVertex3f(0, 0.5f, -1.0f);
        glColor3f(0, 0, 1.0f);
        glVertex3f(0.5f, -0.5f, -1.0f);
        glEnd();
    }

    void idle()
    {
        //angle_ += 0.1f;
    }

    void resize(int w, int h)
    {
        glViewport(0, 0, w, h);
    }

    void key_down(int x, int y, unsigned int key)
    {
        shogl()->window_quit(0);
    }


};

SHOGL_CLASS(myWindow)
{
    shogl()->window_title("Hello triangle (class) GL1");
    shogl()->window_fps(60);
}