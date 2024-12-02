#include <glwindow.hpp>

#include "helpers.hpp"


GLuint vao;
GLuint program;

// Rotation angle...
float angle = 0;

GLuint rotationMatrixLocation;
float rotationMatrix[16];

GLWINDOW(4, 3)
{
    glwindow::width(800);
    glwindow::height(600);
    glwindow::title(L"OpenGL 4.3");

    GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
    GLFN(GLGENBUFFERS, glGenBuffers)
    GLFN(GLBINDBUFFER, glBindBuffer)
    GLFN(GLBUFFERDATA, glBufferData)
    GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
    GLFN(GLDISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray)
    GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
    GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)

    // Create our geometry...
    std::vector<float> points = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
    std::vector<float> colours = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    unsigned int pointBuffer, colourBuffer;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &pointBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    
    glGenBuffers(1, &colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colours.size(), &colours.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);

    glBindVertexArray(NULL);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    GLuint vertexShader = shogl_helpers::glsl_compile(GL_VERTEX_SHADER, R"(
        #version 410 core
        layout(location=0) in vec3 inPoint;
        layout(location=1) in vec3 inColour;
        out vec3 colour;
        uniform mat4 rotationMatrix;
        void main()
        {
                gl_Position = rotationMatrix * vec4(inPoint, 1.0);
            colour = inColour;
        }
    )");

    GLuint fragmentShader = shogl_helpers::glsl_compile(GL_FRAGMENT_SHADER, R"(
        #version 410 core
        in vec3 colour;
        out vec4 frag;
        void main()
        {
            frag = vec4(colour, 1.0);
        }
    )");

    program = shogl_helpers::glsl_link({ vertexShader, fragmentShader });
    rotationMatrixLocation = glGetUniformLocation(program, "rotationMatrix");

    glUseProgram(NULL);

}

GLWINDOW_DRAW
{
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
    GLFN(GLUNIFORMMATRIX4FV, glUniformMatrix4fv)

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, rotationMatrix);
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(NULL);
    glUseProgram(NULL);
}

GLWINDOW_IDLE
{
    // Check the frame limiter, only increment angle if frame time has passed...
    if (glwindow_get()->frame_limiter())
    {
        angle += 0.2f;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle, 0, 0, 1.0f);
        glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
    }
}

GLWINDOW_MOUSE_CLICK(x, y, button)
{
    if (button == glwindow::mouse_button::left)
        GLWINDOW_QUIT(1);
}

GLWINDOW_RESIZE(width, height)
{
    glViewport(0, 0, width, height);
}