#include "Renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

GLuint Renderer::LoadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // <--- Flip nativo padrao
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4); 
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Falha ao carregar textura: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

void Renderer::DrawTexture(GLuint textureID, float x, float y, float width, float height) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset cor

    glBegin(GL_QUADS);
        // Canto inferior esquerdo (Bottom-Left)
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        // Canto inferior direito (Bottom-Right)
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
        // Canto superior direito (Top-Right)
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
        // Canto superior esquerdo (Top-Left)
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Renderer::DrawSemiTransparentRect(float xMin, float xMax, float yMin, float yMax, float r, float g, float b, float a) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(xMin, yMin);
        glVertex2f(xMax, yMin);
        glVertex2f(xMax, yMax);
        glVertex2f(xMin, yMax);
    glEnd();
    glDisable(GL_BLEND);
}

void Renderer::DrawText(const std::string& text, float x, float y, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glColor3f(1.0f, 1.0f, 1.0f); // Reseta a cor
}
