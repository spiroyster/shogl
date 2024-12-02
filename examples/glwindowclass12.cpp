#include <glwindow.hpp>

class myglwindow : public glwindow
{
    float angle = 0;
public:
    myglwindow()
        : glwindow(L"OpenGL 1.2 myWindow", 800, 600, false)
    {
    }

    void draw()
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle += 0.1f, 0, 0, 1.0f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0, 0);
        glVertex3f(-0.5f, -0.5f, -1.0f);
        glColor3f(0, 1.0f, 0);
        glVertex3f(0, 0.5f, -1.0f);
        glColor3f(0, 0, 1.0f);
        glVertex3f(0.5f, -0.5f, -1.0f);
        glEnd();
    }

    void resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void mouse_down(int x, int y, mouse_button button)
    {
        if (button == mouse_button::left)
            GLWINDOW_QUIT(1)
    }
};

GLWINDOW_CLASS(1, 2, myglwindow)
{
    // This is called after context and window creation...
    // can set width, height, name, fps target, click duration
    // but not glversion, fullscreen

}