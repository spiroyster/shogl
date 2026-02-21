#include "../include/shogl.hpp"

namespace
{
    GLuint glsl_compile(GLuint type, const std::string& source)
    {
        GLFN(GLCREATESHADER, glCreateShader)
        GLFN(GLSHADERSOURCE, glShaderSource)
        GLFN(GLCOMPILESHADER, glCompileShader)
        GLFN(GLLINKPROGRAM, glLinkProgram)
        GLFN(GLGETSHADERIV, glGetShaderiv)
        GLFN(GLGETSHADERINFOLOG, glGetShaderInfoLog)

        GLuint shaderID = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(shaderID, 1, &src, NULL);
        glCompileShader(shaderID);

        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int infoLogLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> error(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }
        return shaderID;
    }

    GLuint glsl_link(const std::vector<GLuint>& shaders)
    {
        GLFN(GLCREATEPROGRAM, glCreateProgram)
        GLFN(GLATTACHSHADER, glAttachShader)
        GLFN(GLDETACHSHADER, glDetachShader)
        GLFN(GLLINKPROGRAM, glLinkProgram)
        GLFN(GLGETPROGRAMIV, glGetProgramiv)
        GLFN(GLGETPROGRAMINFOLOG, glGetProgramInfoLog)

        GLuint programID = glCreateProgram();
        for (unsigned int s = 0; s < shaders.size(); ++s)
            glAttachShader(programID, shaders[s]);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int InfoLogLength;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<GLchar> error(InfoLogLength);
            glGetProgramInfoLog(programID, InfoLogLength, &InfoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }

        for (unsigned int s = 0; s < shaders.size(); ++s)
            glDetachShader(programID, shaders[s]);

        return programID;
    }
}

SHOGL()
{
    shogl()->window_title("Hello triangle (class) GL4");
    shogl()->window_fps(60);
    shogl()->window_size(600, 600);

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

    GLuint vao;
    GLuint program;
    unsigned int pointBuffer, colourBuffer;

    // Rotation angle...
    float angle = 0;

    GLuint rotationMatrixLocation;
    float rotationMatrix[16] = { 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    // Create our geometry...
    std::vector<float> points = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
    std::vector<float> colours = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

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

    GLuint vertexShader = glsl_compile(GL_VERTEX_SHADER, R"(
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

    GLuint fragmentShader = glsl_compile(GL_FRAGMENT_SHADER, R"(
            #version 410 core
            in vec3 colour;
            out vec4 frag;
            void main()
            {
                frag = vec4(colour, 1.0);
            }
        )");

    program = glsl_link({ vertexShader, fragmentShader });
    rotationMatrixLocation = glGetUniformLocation(program, "rotationMatrix");

    glUseProgram(NULL);

    shogl()->draw([=, &rotationMatrix, &angle]()
        {
            angle += 0.1f;
            rotationMatrix[0] = static_cast<float>(cos(angle));
            rotationMatrix[1] = static_cast<float>(sin(angle));
            rotationMatrix[4] = static_cast<float>(-sin(angle));
            rotationMatrix[5] = static_cast<float>(cos(angle));
            rotationMatrix[10] = 1.0f;
            rotationMatrix[15] = 1.0f;

            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);
            glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, rotationMatrix);
            glBindVertexArray(vao);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(NULL);
            glUseProgram(NULL);
        });

    shogl()->resize([](int w, int h)
        {
            glViewport(0, 0, w, h);
        });

    shogl()->kill([=]() 
        {
            glDeleteBuffers(1, &pointBuffer);
            glDeleteBuffers(1, &colourBuffer);
            glDeleteVertexArrays(1, &vao);
            glDeleteProgram(program);
        });

    shogl()->key_down([](int x, int y, unsigned int key)
        {
            if (key == VK_ESCAPE)
                shogl()->window_quit(0);
            else
                shogl()->window_event_behaviour(shogl()->window_event_behaviour() == shogl_window::peekEvent ?
                    shogl_window::waitForEvent : shogl_window::peekEvent);
        });


    return shogl()->window_show();
}