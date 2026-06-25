#pragma once

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glew.h>
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <string>

class Renderer {
public:
    static GLuint LoadTexture(const std::string& path);
    static void DrawTexture(GLuint textureID, float x, float y, float width, float height);
    static void DrawSemiTransparentRect(float xMin, float xMax, float yMin, float yMax, float r, float g, float b, float a);
    static void DrawText(const std::string& text, float x, float y, float r, float g, float b);
};
