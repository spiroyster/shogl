#include <glwindow.hpp>

// Rotation angle...
float angle = 0;

GLWINDOW(1, 2)
{
    glwindow::width(800);
    glwindow::height(600);
    glwindow::title(L"OpenGL 1.2");
    glwindow::target_fps(120);
}

GLWINDOW_DRAW
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

GLWINDOW_RESIZE(w, h)
{
    glViewport(0, 0, w, h);
}

GLWINDOW_MOUSE_CLICK(x, y, button)
{
    if (button == glwindow::mouse_button::left)
        GLWINDOW_QUIT(1);
}