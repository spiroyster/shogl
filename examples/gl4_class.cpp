
#include "../include/shogl.hpp"
#include "helpers.hpp"

#include <math.h>

class myWindow : public shogl_window
{
    GLuint vao_;
    GLuint program_;
    unsigned int pointBuffer_, colourBuffer_;

    // Rotation angle...
    float angle_ = 0;

    GLuint rotationMatrixLocation_;
    float rotationMatrix_[16];

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
    GLFN(GLUNIFORMMATRIX4FV, glUniformMatrix4fv)
    GLFN(GLDELETEPROGRAM, glDeleteProgram)
    GLFN(GLDELETEVERTEXARRAYS, glDeleteVertexArrays)
    GLFN(GLDELETEBUFFERS, glDeleteBuffers)

public:
    myWindow()
    {
        // Create our geometry...
        std::vector<float> points = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
        std::vector<float> colours = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
        

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &pointBuffer_);
        glBindBuffer(GL_ARRAY_BUFFER, pointBuffer_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points.front(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, NULL);

        glGenBuffers(1, &colourBuffer_);
        glBindBuffer(GL_ARRAY_BUFFER, colourBuffer_);
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

        program_ = shogl_helpers::glsl_link({ vertexShader, fragmentShader });
        rotationMatrixLocation_ = glGetUniformLocation(program_, "rotationMatrix");

        glUseProgram(NULL);
    }

    void draw()
    {
        angle_ += 0.1f;
        rotationMatrix_[0] = static_cast<float>(cos(angle_));
        rotationMatrix_[1] = static_cast<float>(sin(angle_));
        rotationMatrix_[4] = static_cast<float>(-sin(angle_));
        rotationMatrix_[5] = static_cast<float>(cos(angle_));
        rotationMatrix_[10] = 1.0f;
        rotationMatrix_[15] = 1.0f;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_);
        glUniformMatrix4fv(rotationMatrixLocation_, 1, GL_FALSE, rotationMatrix_);
        glBindVertexArray(vao_);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(NULL);
        glUseProgram(NULL);
    }

    void resize(int w, int h)
    {
        glViewport(0, 0, w, h);
    }

    void kill()
    {
        glDeleteBuffers(1, &pointBuffer_);
        glDeleteBuffers(1, &colourBuffer_);
        glDeleteVertexArrays(1, &vao_);
        glDeleteProgram(program_);
    }

    void key_down(int x, int y, unsigned int key)
    {
        shogl()->window_quit(0);
        printf("quitting...");
    }
};

SHOGL_CLASS(myWindow)
{
    shogl()->window_title("Hello triangle GL4");
    shogl()->window_size(600, 600);
    //shogl()->window_fps(60);
}