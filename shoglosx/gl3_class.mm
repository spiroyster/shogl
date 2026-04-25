// Running example 

#include "../include/shogl.hpp"

#include <OpenGL/gl3.h>
#include <iostream>

class gl3_class : public shogl_window
{
    GLuint vao, vbo, program;

    GLuint compileShader(GLenum type, const char* src) 
    {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        return s;
    }

public:
    gl3_class()
    :   shogl_window()
    {
        const char* vs = R"(
            #version 150
            in vec2 position;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )";

        const char* fs = R"(
            #version 150
            out vec4 color;
            void main() {
                color = vec4(1.0, 0.5, 0.2, 1.0);
            }
        )";

        GLuint vert = compileShader(GL_VERTEX_SHADER, vs);
        GLuint frag = compileShader(GL_FRAGMENT_SHADER, fs);

        program = glCreateProgram();
        glAttachShader(program, vert);
        glAttachShader(program, frag);
        glBindAttribLocation(program, 0, "position");
        glLinkProgram(program);

        float verts[] = {
            0.0f,  0.5f,
            -0.5f, -0.5f,
            0.5f, -0.5f
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        //resize(width, height);
    }

    void draw()
    {
        //std::cout << "gl3_class.cpp:draw\n";

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
};

extern void osx_window_run();
namespace { static std::unique_ptr<gl3_class> instance_; }
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<gl3_class>(); } return instance_.get(); }
int main() 
{
    osx_window_run();
    return 0;
}
